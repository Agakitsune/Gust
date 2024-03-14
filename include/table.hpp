
#pragma once

#include "table.h"
#include "entity.hpp"

namespace gust {

    class World;

    class Table {
        table_t table;

        public:
            Table(const table_t *table);
            ~Table() = default;

            // Type type() const;
            uint64_t blockSize() const;
            uint64_t blockCount() const;
            uint64_t rowSize() const;
            uint64_t count() const;
            uint64_t capacity() const;

            void *data() const;

            int add(void *data);
            void *get(const World &world, uint64_t row, Entity component) const;
            void *get(uint64_t row, uint64_t shift) const;
            int clearRow(uint64_t row);
            void *getRow(uint64_t row) const;
            void *getLastRow() const;
            void *getLastRowForManualInsert();
    };

}
