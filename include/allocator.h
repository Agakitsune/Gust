
#ifndef GUST_ALLOCATOR_H
#define GUST_ALLOCATOR_H

#include <stdint.h>

typedef struct block_t {
    uint64_t size;
    uint32_t is_free;
    uint32_t __pad;
} block_t;

typedef struct page_t {
    void *data;
    block_t *table;
    struct page_t *next;
    uint64_t size;
} page_t;

typedef struct allocator_t {
    page_t *table;
} allocator_t;

/*
** Page size: 4096
** Block size: 64
** Block per page: 64
**
** Block header: 16
** Block table: 1024 -> 16 * 64 (one block header per block)
** Block tables per page: 4
**
** Page header: 32
** Page table: 256 -> 32 * 8 (one page header per block table)
** Page tables per page: 16
**
*/

void allocator_init(allocator_t *allocator);
void allocator_end(allocator_t *allocator);

void *allocator_alloc(allocator_t *allocator, uint64_t size);
void allocator_free(allocator_t *allocator, void *ptr);



void page_init(page_t *page, void *restrict data, block_t *restrict table, uint64_t size);
void page_end(page_t *page);

int page_allocate(page_t *page, uint64_t size);
void page_coalesce(block_t *head, block_t *tail);


void block_init(block_t *block, uint64_t size);
void *block_next(block_t *block);

int block_split(block_t *block);
void block_split_until(block_t *block, uint64_t size);
block_t *block_search(block_t *head, block_t *tail, uint64_t size);

#endif // GUST_ALLOCATOR_H
