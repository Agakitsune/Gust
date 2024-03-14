
#pragma once

#include <string>

#include "entity.hpp"
#include "component.h"

namespace gust {

    class World;

    class Component {
        component_t data;

        public:
            Component(const char *name, uint64_t size);
            Component(const component_t *component);
            ~Component() = default;

            const char *getName() const;
            uint64_t getSize() const;
    };

}

gust::Component getComponent(const gust::World &world, gust::Entity component);
gust::Component getComponent(const gust::World &world, const char *name);
gust::Component getComponent(const gust::World &world, const char *name, size_t n);
gust::Component getComponent(const gust::World &world, const std::string &name);
gust::Component getComponent(const gust::World &world, const std::string &name, size_t n);
gust::Entity getComponentID(const gust::World &world, const char *name);
gust::Entity getComponentID(const gust::World &world, const char *name, size_t n);
gust::Entity getComponentID(const gust::World &world, const std::string &name);
gust::Entity getComponentID(const gust::World &world, const std::string &name, size_t n);
const char *getComponentName(const gust::World &world, gust::Entity component);
uint64_t getComponentSize(const gust::World &world, gust::Entity component);
