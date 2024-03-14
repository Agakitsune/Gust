
#pragma once

#ifdef __linux__
    #include <cstdlib>
    #include <cxxabi.h>
#endif

#include "iter.h"
#include "entity.hpp"

namespace gust {

    class World;

    class Iterator {
        gust_iter_t iter;

        public:
            Iterator(gust_iter_t *iter);
            ~Iterator() = default;

            template<typename T>
            T &field(Entity component) {
                T *field = (T*)gust_get_data(&iter, component);
                return *field;
            }

            template<typename T>
            T &field() {
                const char *name;
                #ifdef __linux__
                    name = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, NULL);
                #else
                    name = strdup(typeid(T).name());
                #endif
                T *field = (T*)gust_get_data_from_name(&iter, name);
                std::free((void*)name);
                return *field;
            }
    };

}

gust::Iterator getIterator(const gust::World &world, gust::Entity iter);
