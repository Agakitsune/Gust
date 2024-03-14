
#pragma once

#include <iostream>

#include "type.h"

#include "entity.hpp"

namespace gust {

    class World;

    class Type {
        type_t type;
        bool owner = true;

        public:
            Type();
            Type(const Type &other);
            Type(Type &&other);
            Type(type_t *type);
            ~Type();

            Type &operator=(const Type &other);
            Type &operator=(Type &&other);

            bool operator==(const Type &other) const;
            bool operator!=(const Type &other) const;
            Entity operator[](uint64_t index) const;

            uint64_t count() const;

            int add(Entity component);
            int remove(Entity component);
            bool has(Entity component) const;
            uint64_t index(Entity component) const;
            uint64_t shift(const World &world, Entity until) const;

    };

}
