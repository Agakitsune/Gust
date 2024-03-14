
#ifndef GUST_CLASSIC_VECTOR_H
#define GUST_CLASSIC_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "allocator/classic.h"

typedef struct cvector_t {
    void *data;
    uint64_t count;
    uint64_t block_count;
} cvector_t;

cvector_t *cvector_init(const classic_t *allocator, cvector_t *vector, uint64_t base_count);

void cvector_free(cvector_t *vector);

int cvector_add(const classic_t *allocator, cvector_t *vector, void *data);
int cvector_add_range(const classic_t *allocator, cvector_t *vector, void *data, uint64_t count);

int cvector_insert(const classic_t *allocator, cvector_t *vector, uint64_t index, void *data);
int cvector_insert_range(const classic_t *allocator, cvector_t *vector, uint64_t index, void *data, uint64_t count);

int cvector_remove(const classic_t *allocator, cvector_t *vector, uint64_t index);
int cvector_remove_range(const classic_t *allocator, cvector_t *vector, uint64_t index, uint64_t count);

void *cvector_data(const cvector_t *vector);
uint64_t cvector_size(const cvector_t *vector);

void *cvector_get_index(const classic_t *allocator, const cvector_t *vector, uint64_t index);

#define cvector_data_type(vector, type) ((type*)cvector_data(&vector))

#define cvector_index_type(allocator, vector, index, type) *((type*)cvector_get_index(allocator, vector, index))

#define cvector_back(allocator, vector) cvector_get_index(allocator, vector, cvector_size(vector) - 1)
#define cvector_back_type(allocator, vector, type) *((type*)(cvector_back(allocator, vector)))

#ifdef __cplusplus
}
#endif

#endif // GUST_CLASSIC_VECTOR_H
