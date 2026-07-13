#define PROBLEM "https://www.spoj.com/problems/SALMAN/"

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

#include "../../src/algorithm/tree/euler_tour.hpp"
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include "../../src/structure/segtree/lazysegtree.hpp"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include "../../src/structure/types/monoid_act.hpp"

namespace{

struct SalaryAggregate{
    long long sum;
    long long minimum;
    long long count;
};

constexpr SalaryAggregate salary_op(SalaryAggregate left, SalaryAggregate right){
    return {
        left.sum + right.sum,
        std::min(left.minimum, right.minimum),
        left.count + right.count
    };
}

constexpr SalaryAggregate salary_identity(){
    return {0, std::numeric_limits<long long>::max(), 0};
}

constexpr SalaryAggregate salary_add_mapping(long long delta, SalaryAggregate aggregate){
    if(aggregate.count == 0){
        return aggregate;
    }
    aggregate.sum += delta * aggregate.count;
    aggregate.minimum += delta;
    return aggregate;
}

constexpr long long salary_add_composition(long long added, long long previous){
    return added + previous;
}

constexpr long long salary_add_identity(){
    return 0;
}

constexpr auto salary_monoid = Monoid_Act<
    salary_op,
    salary_identity,
    salary_add_mapping,
    salary_add_composition,
    salary_add_identity
>{};

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int test = 1; test <= test_count; ++test){
        int employee_count, query_count;
        std::cin >> employee_count >> query_count;

        auto tour = std::make_unique<TreeEulerTour<400000>>(employee_count);
        std::vector<long long> salary(static_cast<std::size_t>(employee_count));
        int root = -1;
        for(int employee = 0; employee < employee_count; ++employee){
            int parent;
            std::cin >> parent >> salary[static_cast<std::size_t>(employee)];
            if(parent == 0){
                root = employee;
            }else{
                tour->add_edge(parent - 1, employee);
            }
        }
        tour->build(root);

        std::vector<SalaryAggregate> preorder(
            static_cast<std::size_t>(employee_count)
        );
        for(int employee = 0; employee < employee_count; ++employee){
            const long long value = salary[static_cast<std::size_t>(employee)];
            preorder[static_cast<std::size_t>(tour->tin(employee))] = {value, value, 1};
        }
        auto tree = std::make_unique<LazySegtree<salary_monoid, 400000>>(preorder);

        std::cout << "Case " << test << ":\n";
        while(query_count-- != 0){
            int type, employee;
            std::cin >> type >> employee;
            --employee;
            const auto [left, right] = tour->subtree_range(employee);
            const SalaryAggregate aggregate = tree->prod(left, right);
            if(type == 1){
                std::cout << aggregate.sum << '\n';
            }else{
                tree->apply(left, right, std::min(1000LL, aggregate.minimum));
            }
        }
    }
}
