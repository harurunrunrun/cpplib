// competitive-verifier: PROBLEM https://www.spoj.com/problems/NITTROAD/

#include <iostream>
#include "../../src/structure/dsu/offline_road_disconnection.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests-- != 0){
        int n;
        std::cin >> n;
        OfflineRoadDisconnection<20000> roads(n);
        for(int edge = 1; edge < n; ++edge){
            int u, v;
            std::cin >> u >> v;
            roads.add_road(u - 1, v - 1);
        }
        int q;
        std::cin >> q;
        while(q-- != 0){
            char type;
            std::cin >> type;
            if(type == 'Q') roads.ask();
            else{
                int road;
                std::cin >> road;
                roads.remove_road(road - 1);
            }
        }
        for(long long answer: roads.solve()) std::cout << answer << '\n';
        std::cout << '\n';
    }
}
