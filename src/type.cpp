
#include "type.hpp"
#include "world.hpp"

namespace gust {

    Type::Type() {
        std::memset(&type, 0, sizeof(type_t));
    }

    Type::Type(const Type &other) {
        copy_type(&type, &other.type);
        owner = false;
    }

    Type::Type(Type &&other) {
        std::memcpy(&type, &other.type, sizeof(type_t));
        std::memset(&other.type, 0, sizeof(type_t));
    }

    Type::Type(type_t *query) {
        std::memcpy(&type, query, sizeof(type_t));
    }

    Type::~Type() {
        if (owner) {
            free_type(&type);
        }
    }

    Type &Type::operator=(const Type &other) {
        if (owner) {
            free_type(&type);
        }
        copy_type(&type, &other.type);
        owner = false;
        return *this;
    }

    Type &Type::operator=(Type &&other) {
        if (owner) {
            free_type(&type);
        }
        std::memcpy(&type, &other.type, sizeof(type_t));
        std::memset(&other.type, 0, sizeof(type_t));
        owner = true;
        return *this;
    }

    bool Type::operator==(const Type &other) const {
        return compare_type(&type, &other.type);
    }

    bool Type::operator!=(const Type &other) const {
        return !compare_type(&type, &other.type);
    }

    Entity Type::operator[](uint64_t index) const {
        return type.ids[index];
    }

    uint64_t Type::count() const {
        return type.count;
    }

    int Type::add(Entity component) {
        return add_type(&type, component);
    }

    int Type::remove(Entity component) {
        return remove_type(&type, component);
    }

    bool Type::has(Entity component) const {
        return has_type(&type, component);
    }

    uint64_t Type::index(Entity component) const {
        return index_type(&type, component);
    }
    
    uint64_t Type::shift(const World &world, Entity until) const {
        return shift_type(world.handle(), &type, until);
    }

}
