
#pragma once

#include "record.h"
#include "table.hpp"

namespace gust {

    class Record {
        entity_record_t record;

        public:
            Record(const entity_record_t *record);
            Record() = default;

            Table table() const;
            uint64_t row() const;
    };

}
