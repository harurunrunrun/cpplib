// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/segment_add_get_min

#include <iostream>
#include <string>
#include <vector>

#include "../../src/structure/convex_hull_trick/segment_li_chao_tree.hpp"

struct SegmentAddGetMinQuery{
    int type;
    long long left;
    long long right;
    long long slope;
    long long intercept;
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int initial_count, query_count;
    std::cin >> initial_count >> query_count;
    std::vector<SegmentAddGetMinQuery> initial(static_cast<std::size_t>(initial_count));
    for(auto& query: initial){
        std::cin >> query.left >> query.right >> query.slope >> query.intercept;
        query.type = 0;
    }
    std::vector<SegmentAddGetMinQuery> queries;
    std::vector<long long> coordinates;
    queries.reserve(static_cast<std::size_t>(query_count));
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            SegmentAddGetMinQuery query{};
            query.type = 0;
            std::cin >> query.left >> query.right >> query.slope >> query.intercept;
            queries.push_back(query);
        }else{
            long long x;
            std::cin >> x;
            queries.push_back({1, x, 0, 0, 0});
            coordinates.push_back(x);
        }
    }

    SegmentLiChaoTree tree(std::move(coordinates));
    for(const auto& segment: initial){
        tree.add_segment(segment.left, segment.right, segment.slope, segment.intercept);
    }
    for(const auto& query: queries){
        if(query.type == 0){
            tree.add_segment(query.left, query.right, query.slope, query.intercept);
        }else{
            const auto answer = tree.query(query.left);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "INFINITY\n";
        }
    }
}
