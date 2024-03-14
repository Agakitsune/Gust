
#include "record.hpp"

namespace gust {

    Record::Record(const entity_record_t *record) : record(*record) {}

    Table Record::table() const {
        return Table(record.table);
    }

    uint64_t Record::row() const {
        return record.row;
    }

}
