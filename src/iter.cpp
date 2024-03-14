
#include "iter.hpp"
#include "world.hpp"

namespace gust {

    Iterator::Iterator(gust_iter_t *iter) : iter(*iter) {}

} // namespace gust

gust::Iterator getIterator(const gust::World &world, gust::Entity iter) {
    return gust::Iterator(gust_get_iter(world.handle(), iter));
}
