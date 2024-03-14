
#include "query.hpp"
#include "world.hpp"

namespace gust {

    Query::Query() {
        std::memset(&query, 0, sizeof(query_t));
    }

    Query::Query(const Query &other) {
        copy_query(&query, &other.query);
        owner = false;
    }

    Query::Query(Query &&other) {
        std::memcpy(&query, &other.query, sizeof(query_t));
        std::memset(&other.query, 0, sizeof(query_t));
    }

    Query::Query(query_t *query) {
        std::memcpy(&this->query, query, sizeof(query_t));
    }

    Query::~Query() {
        if (owner) {
            free_query(&query);
        }
    }

    Query &Query::operator=(const Query &other) {
        if (owner) {
            free_query(&query);
        }
        copy_query(&query, &other.query);
        owner = false;
        return *this;
    }

    Query &Query::operator=(Query &&other) {
        if (owner) {
            free_query(&query);
        }
        std::memcpy(&query, &other.query, sizeof(query_t));
        std::memset(&other.query, 0, sizeof(query_t));
        owner = true;
        return *this;
    }

    bool Query::operator==(const Query &other) const {
        return compare_query(&query, &other.query);
    }

    bool Query::operator!=(const Query &other) const {
        return !compare_query(&query, &other.query);
    }

    Entity Query::operator[](uint64_t index) const {
        return query.ids[index];
    }

    uint64_t Query::count() const {
        return query.count;
    }

    int Query::add(Entity component) {
        return add_query(&query, component);
    }

    int Query::remove(Entity component) {
        return remove_query(&query, component);
    }

    bool Query::has(Entity component) const {
        return has_query(&query, component);
    }

    uint64_t Query::index(Entity component) const {
        return index_query(&query, component);
    }
    
    bool Query::match(const Type &type) const {
        for (uint64_t i = 0; i < type.count(); i++) {
            if (!has_query(&query, type[i])) {
                return false;
            }
        }
        return true;
    }

    uint64_t Query::shift(const World &world, Entity until) const {
        return shift_query(world.handle(), &query, until);
    }

}
