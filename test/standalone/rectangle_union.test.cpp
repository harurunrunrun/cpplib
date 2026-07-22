// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/other/offline_query/rectangle_union.hpp"

using Rect = RectangleUnionRectangle<long long>;

long long brute(const std::vector<Rect>& rectangles){
    long long result = 0;
    for(int x = -10; x < 10; x++){
        for(int y = -10; y < 10; y++){
            bool covered = false;
            for(const auto& rect: rectangles){
                long long x1 = std::min(rect.x1, rect.x2);
                long long x2 = std::max(rect.x1, rect.x2);
                long long y1 = std::min(rect.y1, rect.y2);
                long long y2 = std::max(rect.y1, rect.y2);
                if(x1 <= x && x < x2 && y1 <= y && y < y2) covered = true;
            }
            result += covered;
        }
    }
    return result;
}

void self_test(){
    {
        std::vector<Rect> rectangles = {
            {0, 0, 3, 3},
            {1, 1, 4, 4},
        };
        assert(rectangle_union_area<long long>(rectangles) == 14);
    }
    std::mt19937 rng(20260911);
    for(int step = 0; step < 500; step++){
        std::vector<Rect> rectangles;
        int n = static_cast<int>(rng() % 8);
        for(int i = 0; i < n; i++){
            long long x1 = static_cast<int>(rng() % 15) - 7;
            long long x2 = static_cast<int>(rng() % 15) - 7;
            long long y1 = static_cast<int>(rng() % 15) - 7;
            long long y2 = static_cast<int>(rng() % 15) - 7;
            rectangles.push_back({x1, y1, x2, y2});
        }
        assert(rectangle_union_area<long long>(rectangles) == brute(rectangles));
    }
}

int main(){
    int n;
    if(!(std::cin >> n)){
        self_test();
        return 0;
    }
    std::vector<Rect> rectangles(static_cast<std::size_t>(n));
    for(auto& rect: rectangles){
        std::cin >> rect.x1 >> rect.y1 >> rect.x2 >> rect.y2;
    }
    std::cout << rectangle_union_area<long long>(rectangles) << '\n';
}
