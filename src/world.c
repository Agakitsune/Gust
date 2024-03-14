
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "world.h"

uint64_t shift_type(world_t *world, type_t *type, uint64_t id) {
    uint64_t shift = 0;
    for (uint64_t i = 0; i < type->count; i++) {
        if (type->ids[i] == id) {
            return shift;
        }
        shift += gust_get_component_size(world, type->ids[i]);
    }
    return -1;
}

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

entity_t gust_register_component(world_t *world, uint64_t size, const char *name) {
    uint64_t id = world->last_component++;
    uint64_t index = id >> GUST_COMPONENT_BIT;
    if (index >= cvector_size(&world->component_metadata_pages)) {
        void *page = malloc(GUST_COMPONENT_PAGE_SIZE * sizeof(component_t));
        if (page == NULL) {
            return 0;
        }
        cvector_add(&world->allocators.pool_allocator, &world->component_metadata_pages, &page);
    }
    component_t **pages = cvector_data(&world->component_metadata_pages);
    component_t *page = pages[index];
    component_t *comp = &page[id & GUST_COMPONENT_MASK];
    comp->size = size;
    comp->name = name;
    entity_t hash = murmur64strseedless(name, strlen(name));
    cvector_add(&world->allocators.entity_allocator, &world->component_hash, &hash);
    return id;
}

inline const component_t *gust_get_component_metadata_by_name(const world_t *world, const char *name) {
    return gust_get_component_metadata_by_id(world, gust_get_component_id(world, name));
}

const component_t *gust_get_component_metadata_by_id(const world_t *world, uint64_t id) {
    uint64_t index = id >> GUST_COMPONENT_BIT;
    component_t **pages = cvector_data(&world->component_metadata_pages);
    return &pages[index][id & GUST_COMPONENT_MASK];
}

inline uint64_t gust_get_component_id(const world_t *world, const char *name) {
    return gust_get_component_id_w_n(world, name, strlen(name));
}

uint64_t gust_get_component_id_w_n(const world_t *world, const char *name, size_t n) {
    entity_t hash = murmur64strseedless(name, n);
    entity_t *hashes = cvector_data(&world->component_hash);
    for (uint64_t index = 0; index < cvector_size(&world->component_hash); index++) {
        if (hash == hashes[index]) {
            return index;
        }
    }
    return -1;
}

inline const char *gust_get_component_name(const world_t *world, uint64_t component) {
    return gust_get_component_metadata_by_id(world, component)->name;
}

inline uint64_t gust_get_component_size(const world_t *world, uint64_t component) {
    return gust_get_component_metadata_by_id(world, component)->size;
}

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

int new_query(query_t *query, entity_t component) {
    query->ids = malloc(sizeof(entity_t));
    if (query->ids == NULL) {
        return -1;
    }
    query->ids[0] = component;
    query->count = 1;
    return 0;
}

int copy_query(query_t *dest, const query_t *src) {
    dest->ids = malloc(src->count * sizeof(entity_t));
    if (dest->ids == NULL) {
        return -1;
    }
    dest->count = src->count;
    for (uint64_t i = 0; i < src->count; i++) {
        dest->ids[i] = src->ids[i];
    }
    return 0;
}

int add_query(query_t *query, entity_t component) {
    entity_t *ids = realloc(query->ids, (query->count + 1) * sizeof(entity_t));
    if (ids == NULL) {
        return -1;
    }
    ids[query->count] = component;
    query->ids = ids;
    query->count++;
    return 0;
}

int remove_query(query_t *query, entity_t component) {
    for (uint64_t i = 0; i < query->count; i++) {
        if (query->ids[i] == component) {
            for (uint64_t j = i; j < query->count - 1; j++) {
                query->ids[j] = query->ids[j + 1];
            }
            query->count--;
            return 0;
        }
    }
    return -1;
}

int has_query(const query_t *query, entity_t component) {
    for (uint64_t i = 0; i < query->count; i++) {
        if (query->ids[i] == component) {
            return 1;
        }
    }
    return 0;
}

int compare_query(const query_t *a, const query_t *b) {
    if (a->count != b->count) {
        return 0;
    }
    for (uint64_t i = 0; i < a->count; i++) {
        if (a->ids[i] != b->ids[i]) {
            return 0;
        }
    }
    return 1;
}

uint64_t index_query(const query_t *query, entity_t component) {
    for (uint64_t i = 0; i < query->count; i++) {
        if (query->ids[i] == component) {
            return i;
        }
    }
    return -1;
}

void free_query(query_t *query) {
    free(query->ids);
}

void display_query(const query_t *query) {
    printf("Query: ");
    for (uint64_t i = 0; i < query->count; i++) {
        printf("%lu ", query->ids[i]);
    }
    printf("\n");
}

int query_match_type(const query_t *query, const type_t *type) {
    for (uint64_t i = 0; i < query->count; i++) {
        if (!has_type(type, query->ids[i])) {
            return 0;
        }
    }
    return 1;
}

entity_t gust_new_iter(world_t *world, query_t *query) {
    entity_t iter = world->last_iter++;
    uint64_t index = iter >> GUST_ITER_BIT;
    gust_iter_t **iters = cvector_data(&world->iterators);
    if (index > cvector_size(&world->iterators)) {
        void *page = malloc(GUST_ITER_PAGE_SIZE * sizeof(gust_iter_t));
        if (page == NULL) {
            return 0;
        }
        cvector_add(&world->allocators.pool_allocator, &world->iterators, &page);
    }
    gust_iter_t *it = &iters[index][iter & GUST_ITER_MASK];
    if (gust_iter_init(world, it, query) == -1) {
        return -1;
    }
    return iter;
}

int gust_iter_init(world_t *world, gust_iter_t *it, query_t *query) {
    it->query = *query;
    it->world = world;
    it->row = 0;
    it->shifts = malloc(query->count * sizeof(uint64_t));
    if (it->shifts == NULL) {
        return -1;
    }
    it->actual = NULL;
    it->next = NULL;
    it->index = 0;
    it->data = malloc(query->count * sizeof(void*));
    return 0;
}

void gust_iter_fini(gust_iter_t *it) {
    free_query(&it->query);
    free(it->shifts);
    free(it->data);
}

gust_iter_t *gust_get_iter(world_t *world, entity_t iter) {
    uint64_t index = iter >> GUST_ITER_BIT;
    gust_iter_t **iters = cvector_data(&world->iterators);
    return &iters[index][iter & GUST_ITER_MASK];
}

void estimate_shifts(world_t *world, uint64_t *shifts, query_t *query, table_t *table) {
    for (uint64_t i = 0; i < query->count; i++) {
        shifts[i] = shift_type(world, &table->type, query->ids[i]);
    }
}

void put_data(gust_iter_t *it) {
    table_t *table = it->actual;
    uint64_t row = it->row;
    for (uint64_t i = 0; i < it->query.count; i++) {
        it->data[i] = gust_get_data_from_table_w_shift(table, row, it->shifts[i]);
    }
}

void prepare_iter(world_t *world, gust_iter_t *it) {
    uint64_t i = 0;
    table_t **tables = cvector_data(&world->tables);
    if (it->actual == NULL) {
        for (; i < cvector_size(&world->tables); i++) {
            if (query_match_type(&it->query, &(tables[i])->type)) {
                it->actual = tables[i];
                it->row = 0;
                estimate_shifts(world, it->shifts, &it->query, it->actual);
                break;
            }
        }
    }
    i++;
    if (it->next == NULL) {
        for (; i < cvector_size(&world->tables); i++) {
            if (query_match_type(&it->query, &(tables[i])->type)) {
                it->next = tables[i];
                it->index = i;
                break;
            }
        }
    }
    put_data(it);
}

void gust_iter_get_next_table(world_t *world, gust_iter_t *it) {
    uint64_t i = it->index + 1;
    table_t **tables = cvector_data(&world->tables);
    for (; i < cvector_size(&world->tables); i++) {
        if (query_match_type(&it->query, &(tables[i])->type)) {
            it->next = tables[i];
            it->index = i;
            return;
        }
    }
    it->next = NULL;
}

int gust_iter_next(world_t *world, gust_iter_t *it) {
    if (it->actual == NULL) {
        return 0;
    }
    it->row++;
    if (it->row == it->actual->count) {
        it->actual = it->next;
        if (it->actual == NULL) {
            return 0;
        }
        it->row = 0;
        estimate_shifts(world, it->shifts, &it->query, it->actual);
        gust_iter_get_next_table(world, it);
    }
    put_data(it);
    return 1;
}

inline void *gust_get_data(gust_iter_t *it, uint64_t index) {
    return it->data[index];
}

inline void *gust_get_data_from_name(gust_iter_t *it, const char *name) {
    return gust_get_data(it, index_query(&it->query, gust_get_component_id(it->world, name)));
}

int gust_generate_query(world_t *world, query_t *query, const char *str) {
    size_t len = 0;
    uint64_t id = 0;
    while (1) {
        len = strcspn(str, " ,");
        id = gust_get_component_id_w_n(world, str, len);
        if (id == -1) {
            return -1;
        }
        if (add_query(query, id) == -1) {
            return -1;
        }
        str += len;
        if (!str[0]) {
            break;
        }
        str += strspn(str, " ,");
    }
    return 0;
}

entity_t gust_declare_system(world_t *world, void (*system)(gust_iter_t*), const char *query) {
    query_t q = {NULL, 0};
    if (gust_generate_query(world, &q, query) == -1) {
        return 0;
    }
    if (gust_new_iter(world, &q) == -1) {
        free_query(&q);
        return 0;
    }
    return gust_new_system(world, system);
}

entity_t gust_new_system(world_t *world, void (*system)(gust_iter_t*)) {
    entity_t sysindex = world->last_system++;
    uint64_t index = sysindex >> GUST_SYSTEM_BIT;
    system_t **systems = cvector_data(&world->systems);
    if (index > cvector_size(&world->systems)) {
        void *page = malloc(GUST_SYSTEM_PAGE_SIZE * sizeof(system_t));
        if (page == NULL) {
            return 0;
        }
        cvector_add(&world->allocators.pool_allocator, &world->systems, &page);
    }
    system_t *sys = &systems[index][sysindex & GUST_SYSTEM_MASK];
    sys->system = system;
    return sysindex;
}

system_t *gust_get_system(world_t *world, entity_t system) {
    uint64_t index = system >> GUST_SYSTEM_BIT;
    system_t **systems = cvector_data(&world->systems);
    return &systems[index][system & GUST_SYSTEM_MASK];
}

table_t *gust_add_table(world_t *world, type_t *type, uint64_t block_size) {
    // table_t **tables = cvector_data(&world->tables);
    table_t *table = malloc(sizeof(table_t));
    if (table == NULL) {
        return NULL;
    }
    cvector_add(&world->allocators.pool_allocator, &world->tables, &table);
    if (gust_init_table(world, table, type, block_size) == -1) {
        free(table);
        return NULL;
    }
    return table;
}

int gust_init_table(world_t *world, table_t *table, type_t *type, uint64_t block_size) {
    uint64_t row_size = 0;
    for (uint64_t i = 0; i < type->count; i++) {
        row_size += gust_get_component_size(world, type->ids[i]);
    }
    copy_type(&table->type, type);
    table->block_size = block_size;
    table->block_count = 1;
    table->row_size = row_size;
    table->count = 0;
    uint64_t size = block_size;
    if (isPower2(row_size)) {
        table->capacity = size >> log2i(row_size);
    } else {
        table->capacity = size / row_size;
    }
    table->data = malloc(size);
    if (table->data == NULL) {
        return -1;
    }
    return 0;
}

void gust_fini_table(table_t *table) {
    free(table->data);
    free_type(&table->type);
}

table_t *gust_get_table(world_t *world, type_t *type) {
    table_t **tables = cvector_data(&world->tables);
    for (uint64_t i = 0; i < cvector_size(&world->tables); i++) {
        if (compare_type(&tables[i]->type, type)) {
            return tables[i];
        }
    }
    return NULL;
}

int gust_add_to_table(table_t *table, void *data) {
    if (table->count == table->capacity) {
        uint64_t size = table->block_size * ++table->block_count;
        if (isPower2(table->row_size)) {
            table->capacity = size >> log2i(table->row_size);
        } else {
            table->capacity = size / table->row_size;
        }
        table->data = realloc(table->data, size);
        if (table->data == NULL) {
            return -1;
        }
    }
    memcpy((char*)table->data + table->count * table->row_size, data, table->row_size);
    table->count++;
    return 0;
}

void *gust_get_data_from_table_w_id(world_t *world, table_t *table, uint64_t row, uint64_t id) {
    uint64_t shift = shift_type(world, &table->type, id);
    if (shift == -1) {
        return NULL;
    }
    return gust_get_data_from_table_w_shift(table, row, shift);
}

inline void *gust_get_data_from_table_w_shift(table_t *table, uint64_t row, uint64_t shift) {
    return (char*)table->data + row * table->row_size + shift;
}

int gust_clear_row_from_table(table_t *table, uint64_t row) {
    if (row >= table->count) {
        return -1;
    }
    void *last = gust_get_last_row(table);
    void *current = gust_get_row(table, row);
    memmove(current, (char*)current + table->row_size, (char*)last - (char*)current - table->row_size);
    table->count--;
    return 0;
}

inline void *gust_get_row(table_t *table, uint64_t row) {
    return (char*)table->data + row * table->row_size;
}

inline void *gust_get_last_row(table_t *table) {
    return (char*)table->data + table->count * table->row_size;
}

void *gust_get_last_row_for_manual_insert(table_t *table) {
    if (table->count == table->capacity) {
        uint64_t size = table->block_size * ++table->block_count;
        if (isPower2(table->row_size)) {
            table->capacity = size >> log2i(table->row_size);
        } else {
            table->capacity = size / table->row_size;
        }
        table->data = realloc(table->data, size);
        if (table->data == NULL) {
            return NULL;
        }
    }
    return gust_get_last_row(table);
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
