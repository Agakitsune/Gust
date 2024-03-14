
#pragma once

#ifdef __linux__
    #include <cstdlib>
    #include <cxxabi.h>
#endif

#include <iostream>
#include <cstring>
#include <functional>

#include "world.h"
#include "query.h"
#include "component.h"
#include "system.h"

#include "entity.hpp"
#include "iter.hpp"

namespace gust {

    class World {
        world_t world;

        template<typename... T>
        query_t generateQuery() {
            query_t query;
            const char *name;
            ((
                #ifdef __linux__
                    name = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, NULL),
                #else
                    name = strdup(typeid(T).name()),
                #endif
                add_query(&query, gust_get_component_id(&world, name)),
                std::free((void*)name)
            ), ...);
            return query;
        }

        public:
            World();
            ~World();

            Entity newEntity();
            void newEntity(Entity &entity);
            void newEntityBatch(Entity *entities, size_t count);
            void removeEntity(Entity entity);
            void removeEntityBatch(Entity *entities, size_t count);

            template<typename T>
            Entity component() {
                const char *name;
                #ifdef __linux__
                    name = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, NULL);
                #else
                    name = strdup(typeid(T).name());
                #endif
                if (name == nullptr) {
                    return 0;
                }
                return gust_register_component(&world, sizeof(T), name);
            }

            template<typename T>
            void addComponent(Entity entity, T &&component) {
                const char *name;
                #ifdef __linux__
                    name = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, NULL);
                #else
                    name = strdup(typeid(T).name());
                #endif
                if (name == nullptr) {
                    return;
                }
                uint64_t id = gust_get_component_id(&world, name);
                gust_set_component(&world, entity, id, &component);
                std::free((void*)name);
            }

            template<typename... T>
            Entity system(void (*system)(Iterator &it)) {
                query_t query = generateQuery<T...>();
                return gust_declare_system_w_query(&world, (void(*)(gust_iter_t*))system, &query);
            }

            template<typename... T>
            Entity system(const std::function<void(Iterator&)> &func) {
                query_t query = generateQuery<T...>();
                return gust_declare_system_w_query(&world, (void(*)(gust_iter_t*))func.target<void(*)(Iterator&)>(), &query);
            }

            bool update();

            world_t *handle();
            const world_t *handle() const;
    };

}
