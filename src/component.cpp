
#include "component.hpp"
#include "world.hpp"

namespace gust {

    Component::Component(const char *name, uint64_t size) : data({size, name}) {

    }

    Component::Component(const component_t *component) {
        data = *component;
    }

    const char *Component::getName() const {
        return data.name;
    }

    uint64_t Component::getSize() const {
        return data.size;
    }

}

gust::Component getComponent(const gust::World &world, gust::Entity component) {
    return gust::Component(gust_get_component_metadata_by_id(world.handle(), component));
}

gust::Component getComponent(const gust::World &world, const char *name) {
    return gust::Component(gust_get_component_metadata_by_name(world.handle(), name));
}

gust::Component getComponent(const gust::World &world, const char *name, size_t n) {
    return gust::Component(gust_get_component_metadata_by_id(world.handle(), gust_get_component_id_w_n(world.handle(), name, n)));
}

gust::Component getComponent(const gust::World &world, const std::string &name) {
    return gust::Component(gust_get_component_metadata_by_name(world.handle(), name.c_str()));
}

gust::Component getComponent(const gust::World &world, const std::string &name, size_t n) {
    return gust::Component(gust_get_component_metadata_by_id(world.handle(), gust_get_component_id_w_n(world.handle(), name.c_str(), n)));
}

gust::Entity getComponentID(const gust::World &world, const char *name) {
    return gust_get_component_id(world.handle(), name);
}

gust::Entity getComponentID(const gust::World &world, const char *name, size_t n) {
    return gust_get_component_id_w_n(world.handle(), name, n);
}

gust::Entity getComponentID(const gust::World &world, const std::string &name) {
    return gust_get_component_id(world.handle(), name.c_str());
}

gust::Entity getComponentID(const gust::World &world, const std::string &name, size_t n) {
    return gust_get_component_id_w_n(world.handle(), name.c_str(), n);
}

const char *getComponentName(const gust::World &world, gust::Entity component) {
    return gust_get_component_name(world.handle(), component);
}

uint64_t getComponentSize(const gust::World &world, gust::Entity component) {
    return gust_get_component_size(world.handle(), component);
}
