
#include <iostream>

#include "world.hpp"
#include "component.hpp"

struct Velocity {
    float x, y;
};

void move(gust::Iterator &it) {
    Velocity &vel = it.field<Velocity>();
    std::cout << "Velocity: " << vel.x << ", " << vel.y << std::endl;
}

int main() {
    gust::World world;

    gust::Entity entity = world.newEntity();

    world.component<Velocity>();
    gust::Component component = getComponent(world, "Velocity");

    std::cout << component.getName() << std::endl;
    std::cout << component.getSize() << std::endl;

    world.system<Velocity>(move);
    world.addComponent(entity, Velocity{1.0f, 1.0f});
    world.update();
    return 0;
}
