
#include <sys/mman.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "allocator.h"
#include "asm.h"

void allocator_init(allocator_t *allocator) {
    void *ptr = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return;
    }
    allocator->table = ptr;
    page_init(allocator->table, NULL, NULL, 0); // setting NULL to avoid memory consumption
}

inline void allocator_end(allocator_t *allocator) {
    page_end(allocator->table);
}

void *allocator_alloc(allocator_t *allocator, uint64_t size) {
    if (size == 0)
        return NULL;
    
    // Get the greater multiple of 4096 to multipage the allocation
    const uint64_t page_size = NGMn(size, 4096);
    
    page_t *page = allocator->table;
    page_t *page_head = page;
    page_t *page_tail = (page_t*)((char*)page_head + 4096);
    if (page->table == NULL) {
        if (page_allocate(page, page_size) == -1)
            return NULL;
    }

    for (;;) {
        block_t *head = page->table;
        if (head == NULL) {
            if (page_allocate(page, page_size) == -1) // Allocate the page if it was released
                return NULL;
        }

        block_t *best = block_search(head, (block_t*)((char*)head + (page->size >> 2)), size);

        if (best == NULL) {
            if (page->next == NULL) {
                page->next = &page[1];
                if (page->next == page_tail) { // Out of page
                    page_t *new_page = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                    if (new_page == MAP_FAILED) {
                        return NULL;
                    }
                    page->next = new_page;
                    page_head = new_page;
                    page_tail = (page_t*)((char*)page_head + 4096);
                }
                if (page_allocate(page->next, page_size) == -1) {
                    return NULL;
                }
            } else if ((&page[1]) == page_tail) { // Check if the page is the last one, so that we can move the tail to the next page table
                page_head = page->next;
                page_tail = (page_t*)((char*)page_head + 4096);
            }
            page = page->next;
            continue;
        }

        size_t offset = (char *)best - (char *)head;
        offset <<= 2; // Multiply by 4 to get the offset in bytes

        best->is_free = 0;
        return (void *)((char *)page->data + offset);
    }
    return NULL;
}

void allocator_free(allocator_t *allocator, void *ptr) {
    page_t *page = allocator->table;

    do {
        void *start = page->data;
        void *end = (char *)start + page->size;

        if (ptr >= start && ptr < end) {
            block_t *block = page->table;
            block_t *tail = (block_t*)((char *)block + (page->size >> 2));

            while (block < tail) {
                if ((char *)ptr == (char *)page->data + ((char *)block - (char *)page->table)) {
                    block->is_free = 1;
                    page_coalesce(page->table, tail); // Coalesce the page
                    return;
                }
                block = block_next(block);
            }
        }

    } while ((page = page->next) != NULL);
}

void page_init(page_t *page, void *restrict data, block_t *restrict table, uint64_t size) {
    page->data = data;
    page->table = table;
    page->next = NULL;
    page->size = size;
}

void page_end(page_t *page) {
    page_t *head = page;
    page_t *tail = (page_t*)((char*)head + 4096);

    for (;;) {
        int release = 0;  

        munmap(page->data, page->size);
        munmap(page->table, page->size >> 2);
        
        if ((&page[1]) == tail) {
            release = 1;
        }
        page = page->next;
        if (page == NULL)
            break;
        if (release) {
            munmap(head, 4096);
            head = page;
            tail = (page_t*)((char*)head + 4096);
        }
    }
    munmap(head, 4096);
}

int page_allocate(page_t *page, uint64_t size) {
    uint64_t block_table_size = size >> 2; // Divide by 16 to get the size in blocks
    void *data = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED) {
        return - 1;
    }
    block_t *table = mmap(0, block_table_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); // preallocate future tables
    if (table == MAP_FAILED) {
        munmap(data, size);
        return -1;
    }
    page_init(page, data, table, size);
    memset(table, 0, block_table_size);
    memset(data, 0, size);
    block_init(table, size);
    return 0;
}

void page_coalesce(block_t *head, block_t *tail) {
    for (;;) {
        block_t *block = head;
        block_t *buddy = block_next(block);

        int coalesced = 0;
        while (block < tail && buddy < tail) {
            if ((block->size == buddy->size) && block->is_free && buddy->is_free) {
                block->size <<= 1;
                block = block_next(block);
                if (block < tail) {
                    buddy = block_next(block);
                    coalesced = 1;
                }
            } else if (block->size > buddy->size) {
                block = buddy;
                buddy = block_next(block);
            } else {
                block = block_next(block);
                if (block < tail) {
                    buddy = block_next(block);
                }
            }
        }

        if (!coalesced) {
            break;
        }
    }
}

inline void block_init(block_t *block, uint64_t size) {
    block->size = size;
    block->is_free = 1;
}

inline void *block_next(block_t *block) {
    return (void *)((char *)block + (block->size >> 2)); // Divide by 8 to stay into the block table
}

int block_split(block_t *block) {
    if (block->size <= 64) // 64 is the minimum block size (4096 / 64 = 64 blocks per page)
        return -1;
    block->size >>= 1;
    block_t *next = block_next(block);
    block_init(next, block->size);
    return 0;
}

void block_split_until(block_t *block, uint64_t size) {
    while ((block->size >> 1) >= size) {
        if (block_split(block) != 0)
            return;
    }
}

block_t *block_search(block_t *head, block_t *tail, uint64_t size) {
    block_t *block = head;
    block_t *buddy = block_next(block);

    if (buddy == tail && block->is_free) {
        block_split_until(block, size);
        return head;
    }

    while (block < tail && buddy < tail) {
        if (block->is_free && block->size >= size) {
            block_split_until(block, size);
            return block;
        }
        block = buddy;
        buddy = block_next(block);
    }
    return NULL;
}
