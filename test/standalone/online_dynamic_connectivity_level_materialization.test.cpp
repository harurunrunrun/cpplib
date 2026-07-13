// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>
#include "../../src/structure/graph/online_dynamic_connectivity.hpp"

namespace allocation_monitor{

std::size_t allocated_bytes = 0;
bool enabled = false;

void record(std::size_t size) noexcept{
    if(enabled) allocated_bytes += size;
}

void reset() noexcept{
    allocated_bytes = 0;
    enabled = true;
}

std::size_t stop() noexcept{
    enabled = false;
    return allocated_bytes;
}

} // namespace allocation_monitor

#if defined(__GNUC__) || defined(__clang__)
#define ODC_NOINLINE __attribute__((noinline))
#else
#define ODC_NOINLINE
#endif

ODC_NOINLINE void* operator new(std::size_t size){
    if(size == 0) size = 1;
    if(void* pointer = std::malloc(size)){
        allocation_monitor::record(size);
        return pointer;
    }
    throw std::bad_alloc();
}

ODC_NOINLINE void* operator new[](std::size_t size){
    return ::operator new(size);
}

ODC_NOINLINE void operator delete(void* pointer) noexcept{
    std::free(pointer);
}

ODC_NOINLINE void operator delete[](void* pointer) noexcept{
    ::operator delete(pointer);
}

ODC_NOINLINE void operator delete(void* pointer, std::size_t) noexcept{
    std::free(pointer);
}

ODC_NOINLINE void operator delete[](void* pointer, std::size_t) noexcept{
    ::operator delete(pointer);
}

#undef ODC_NOINLINE

int main(){
    constexpr int max_size = 262144;
    constexpr std::size_t level_creation_allocation_limit = 1U << 20;

    int n;
    if(!(std::cin >> n)) return 0;
    assert(4 <= n && n <= max_size);

    auto graph = std::make_unique<OnlineDynamicConnectivity<max_size>>(n);
    std::cout << graph->component_size(n - 1) << ' '
              << graph->same(0, n - 1) << '\n';

    std::cout << graph->add_edge(0, 1) << ' '
              << graph->add_edge(1, 2) << ' '
              << graph->add_edge(2, 3) << '\n';

    allocation_monitor::reset();
    bool erased = graph->erase_edge(1, 2);
    std::size_t level_creation_allocation = allocation_monitor::stop();

    assert(level_creation_allocation < level_creation_allocation_limit);
    std::cout << erased << ' '
              << graph->active_levels() << ' '
              << graph->groups() << ' '
              << graph->same(0, 3) << ' '
              << graph->component_size(0) << ' '
              << graph->component_size(3) << '\n';
}
