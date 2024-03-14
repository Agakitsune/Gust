
#pragma once

#include <iostream>

#include "query.h"

#include "entity.hpp"
#include "type.hpp"

namespace gust {

    class World;

    class Query {
        query_t query;
        bool owner = true;

        public:
            Query();
            Query(const Query &other);
            Query(Query &&other);
            Query(query_t *query);
            ~Query();

            Query &operator=(const Query &other);
            Query &operator=(Query &&other);

            bool operator==(const Query &other) const;
            bool operator!=(const Query &other) const;
            Entity operator[](uint64_t index) const;

            uint64_t count() const;

            int add(Entity component);
            int remove(Entity component);
            bool has(Entity component) const;
            uint64_t index(Entity component) const;
            bool match(const Type &type) const;
            uint64_t shift(const World &world, Entity until) const;

    };

}
