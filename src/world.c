
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "world.h"
#include "record.h"
#include "system.h"
#include "iter.h"
#include "table.h"
#include "type.h"
#include "component.h"

world_t *gust_init(world_t *world) {
    if (world == NULL)
        world = malloc(sizeof(world_t));
    if (world == NULL) {
        return NULL;
    }
    world->allocators.entity_allocator = classic_init(entity_t, 512);
    world->allocators.pool_allocator = classic_init(void*, 64);
    cvector_init(&world->allocators.entity_allocator, &world->free_entity, 64);
    cvector_init(&world->allocators.pool_allocator, &world->entity_atlas, 64);
    
    cvector_init(&world->allocators.pool_allocator, &world->component_metadata_pages, 64);
    cvector_init(&world->allocators.entity_allocator, &world->component_hash, 64);
    
    cvector_init(&world->allocators.pool_allocator, &world->tables, 64);
    
    cvector_init(&world->allocators.pool_allocator, &world->iterators, 64);
    cvector_init(&world->allocators.pool_allocator, &world->systems, 64);

    void *page = malloc(GUST_COMPONENT_PAGE_SIZE * sizeof(component_t));
    if (page == NULL) {
        return NULL;
    }
    cvector_add(&world->allocators.pool_allocator, &world->component_metadata_pages, &page);

    page = malloc(GUST_ENTITY_PAGE_SIZE * sizeof(entity_record_t));
    if (page == NULL) {
        return NULL;
    }
    cvector_add(&world->allocators.pool_allocator, &world->entity_atlas, &page);

    page = malloc(GUST_ITER_PAGE_SIZE * sizeof(gust_iter_t));
    if (page == NULL) {
        return NULL;
    }
    cvector_add(&world->allocators.pool_allocator, &world->iterators, &page);

    page = malloc(GUST_SYSTEM_PAGE_SIZE * sizeof(system_t));
    if (page == NULL) {
        return NULL;
    }
    cvector_add(&world->allocators.pool_allocator, &world->systems, &page);

    world->last_entity = 0;
    world->last_component = 0;
    return world;
}

void gust_fini(const world_t *world) {
    cvector_free(&world->free_entity);
    void **pages = cvector_data(&world->entity_atlas);
    for (uint64_t i = 0; i < cvector_size(&world->entity_atlas); i++) {
        free(pages[i]);
    }
    cvector_free(&world->entity_atlas);
    pages = cvector_data(&world->component_metadata_pages);
    for (uint64_t i = 0; i < cvector_size(&world->component_metadata_pages); i++) {
        free(pages[i]);
    }
    cvector_free(&world->component_metadata_pages);
    cvector_free(&world->component_hash);
    pages  = cvector_data(&world->tables);
    for (uint64_t i = 0; i < cvector_size(&world->tables); i++) {
        gust_fini_table(pages[i]);
        free(pages[i]);
    }
    cvector_free(&world->tables);

    uint64_t index = 0;

    for (; index < world->last_iter; index++) {
        gust_iter_fini(gust_get_iter(world, index));
    }

    pages = cvector_data(&world->iterators);
    for (uint64_t i = 0; i < cvector_size(&world->iterators); i++) {
        free(pages[i]);
    }
    cvector_free(&world->iterators);
    pages = cvector_data(&world->systems);
    for (uint64_t i = 0; i < cvector_size(&world->systems); i++) {
        free(pages[i]);
    }
    cvector_free(&world->systems);
}

int gust_update(world_t *world) {
    for (entity_t i = 0; i < world->last_system; i++) {
        system_t *sys = gust_get_system(world, i);
        gust_iter_t *it = gust_get_iter(world, i);
        if (it == NULL || sys == NULL) {
            continue;
        }
        prepare_iter(world, it);
        if (it->actual == NULL) {
            continue;
        }
        do {
            sys->system(it);
        } while (gust_iter_next(world, it));
    }
    return 1;
}

int gust_set_component(world_t *world, entity_t entity, uint64_t component, void *data) {
    entity_record_t *record = gust_get_record(world, entity);
    if (record == NULL) {
        return -1;
    }
    
    uint64_t newborn = 0;
    table_t *table = record->table;
    if (table == NULL) {
        newborn = 1;
        type_t type;
        type.ids = &component;
        type.count = 1;
        table = gust_get_table(world, &type);
        if (table == NULL) {
            table = gust_add_table(world, &type, 4096);
            if (table == NULL) {
                return -1;
            }
        }
        record->table = table;
    }

    uint64_t composite = table->type.count > 1;
    uint64_t modify = has_type(&table->type, component);

    if (newborn) {
        if (composite) {
            return -1; // A newborn entity cannot have another component
        }
        if (gust_add_to_table(table, data) == -1) {
            return -1;
        }
        record->row = table->count - 1;
        return 0;
    }

    if (modify) { // Modify the table
        uint64_t shift = shift_type(world, &table->type, component);
        void *dest = gust_get_data_from_table_w_shift(table, record->row, shift);
        memcpy(dest, data, gust_get_component_size(world, component));
        return 0;
    } else { // Move the entity to a new table
        type_t type;
        copy_type(&type, &table->type);
        if (add_type(&type, component) == -1) {
            free_type(&type);
            return -1;
        }
        uint64_t index = index_type(&type, component);
        uint64_t before = 0;
        uint64_t after = 0;
        for (uint64_t i = 0; i < index; i++) {
            before += gust_get_component_size(world, table->type.ids[i]);
        }
        for (uint64_t i = index + 1; i < table->type.count; i++) {
            after += gust_get_component_size(world, table->type.ids[i]);
        }
        table_t *new_table = gust_get_table(world, &type);
        if (new_table == NULL) {
            new_table = gust_add_table(world, &type, 4096);
            if (new_table == NULL) {
                free_type(&type);
                return -1;
            }
        }
        free_type(&type);
        void *src = gust_get_row(table, record->row);
        void *dest = gust_get_last_row_for_manual_insert(new_table);

        memcpy(dest, src, before);
        memcpy((char*)dest + before, data, gust_get_component_size(world, component));
        memcpy((char*)dest + before + gust_get_component_size(world, component), (char*)src + before, after);

        record->table = new_table;
        record->row = new_table->count;
        new_table->count++;
        return 0;
    }
    return 0;
}
