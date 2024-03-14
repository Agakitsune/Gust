
#include "record.h"
#include "world.h"

entity_record_t *gust_add_record(world_t *world, entity_t entity) {
    uint64_t index = entity >> GUST_ENTITY_BIT;
    if (index >= cvector_size(&world->entity_atlas)) {
        void *page = malloc(GUST_ENTITY_PAGE_SIZE * sizeof(entity_record_t));
        if (page == NULL) {
            return NULL;
        }
        cvector_add(&world->allocators.pool_allocator, &world->entity_atlas, &page);
    }
    entity_record_t **records = cvector_data(&world->entity_atlas);
    entity_record_t *record = &records[index][entity & GUST_ENTITY_MASK];
    record->table = NULL;
    record->row = 0;
    return record;
}

int gust_init_record(entity_record_t *record, table_t *table) {
    record->table = table;
    record->row = 0;
    return 0;
}

entity_record_t *gust_get_record(world_t *world, entity_t entity) {
    uint64_t index = entity >> GUST_ENTITY_BIT;
    entity_record_t **records = cvector_data(&world->entity_atlas);
    return &records[index][entity & GUST_ENTITY_MASK];
}

void gust_remove_record(world_t *world, entity_t entity) {
    entity_record_t *record = gust_get_record(world, entity);
    if (record->table != NULL) {
        gust_clear_row_from_table(record->table, record->row);
    }
    record->table = NULL;
    record->row = 0;
}
