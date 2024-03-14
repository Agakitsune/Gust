
#include "table.hpp"
#include "world.hpp"

namespace gust {

    Table::Table(const table_t *table) : table(*table) {}

    // Type type() const;
    uint64_t Table::blockSize() const {
        return table.block_size;
    }

    uint64_t Table::blockCount() const {
        return table.block_count;
    }

    uint64_t Table::rowSize() const {
        return table.row_size;
    }

    uint64_t Table::count() const {
        return table.count;
    }

    uint64_t Table::capacity() const {
        return table.capacity;
    }

    void *Table::data() const {
        return table.data;
    }

    int Table::add(void *data) {
        return gust_add_to_table(&table, data);
    }

    void *Table::get(const World &world, uint64_t row, Entity component) const {
        return gust_get_data_from_table_w_id(world.handle(), &table, row, component);
    }

    void *Table::get(uint64_t row, uint64_t shift) const {
        return gust_get_data_from_table_w_shift(&table, row, shift);
    }

    int Table::clearRow(uint64_t row) {
        return gust_clear_row_from_table(&table, row);
    }

    void *Table::getRow(uint64_t row) const {
        return gust_get_row(&table, row);
    }

    void *Table::getLastRow() const {
        return gust_get_last_row(&table);
    }

    void *Table::getLastRowForManualInsert() {
        return gust_get_last_row_for_manual_insert(&table);
    }

}
