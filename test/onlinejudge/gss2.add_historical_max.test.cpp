// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS2/

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

struct Query{
    int left;
    int right;
    int index;
};

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int element_count;
    std::cin >> element_count;
    std::vector<int> values(static_cast<std::size_t>(element_count));
    for(int& value: values) std::cin >> value;

    int query_count;
    std::cin >> query_count;
    std::vector<Query> queries(static_cast<std::size_t>(query_count));
    for(int index = 0; index < query_count; ++index){
        int left, right;
        std::cin >> left >> right;
        queries[static_cast<std::size_t>(index)] = {left - 1, right - 1, index};
    }
    std::sort(queries.begin(), queries.end(), [](const Query& left, const Query& right){
        return left.right < right.right;
    });

    std::vector<HistoricalMaxAggregate<long long>> initial(
        static_cast<std::size_t>(element_count),
        {0, 0}
    );
    auto range = std::make_unique<LazySegtree<
        AddHistoricalMaxMonoidAct<long long>{},
        100000
    >>(initial);

    std::vector<int> last_occurrence(200001, -1);
    std::vector<long long> answer(static_cast<std::size_t>(query_count));
    int processed = 0;
    for(const Query& query: queries){
        while(processed <= query.right){
            const int value = values[static_cast<std::size_t>(processed)];
            const int value_index = value + 100000;
            range->apply(
                last_occurrence[static_cast<std::size_t>(value_index)] + 1,
                processed + 1,
                historical_add(static_cast<long long>(value))
            );
            last_occurrence[static_cast<std::size_t>(value_index)] = processed;
            ++processed;
        }
        answer[static_cast<std::size_t>(query.index)] =
            range->prod(query.left, query.right + 1).historical;
    }

    for(long long value: answer) std::cout << value << '\n';
}
