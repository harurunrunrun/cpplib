// competitive-verifier: STANDALONE

#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "../../src/algorithm/geometry/geometry.hpp"

void self_test(){
    assert(ccw({0, 0}, {2, 0}, {1, 1}) == COUNTER_CLOCKWISE);
    assert(ccw({0, 0}, {2, 0}, {1, -1}) == CLOCKWISE);
    assert(ccw({0, 0}, {2, 0}, {1, 0}) == ON_SEGMENT);
    assert(intersect(Segment{{0, 0}, {2, 2}}, Segment{{0, 2}, {2, 0}}));
    assert(!intersect(Segment{{0, 0}, {1, 0}}, Segment{{2, 0}, {3, 0}}));
    Point p = projection(Line{{0, 0}, {2, 0}}, {1, 3});
    assert(p == Point(1, 0));
    std::vector<Point> polygon = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    assert(contains(polygon, {2, 2}) == 2);
    assert(contains(polygon, {4, 2}) == 1);
    assert(contains(polygon, {5, 2}) == 0);
    auto hull = convex_hull({{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0.5L, 0.5L}});
    assert(hull.size() == 4);
    assert(geometry_sign(area(hull) - 1.0L) == 0);
    assert(is_convex(hull));
}

int main(){
    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "CCW"){
            Point a, b, c;
            std::cin >> a.x >> a.y >> b.x >> b.y >> c.x >> c.y;
            std::cout << ccw(a, b, c) << '\n';
        }else if(type == "INTERSECT"){
            Segment s, t;
            std::cin >> s.a.x >> s.a.y >> s.b.x >> s.b.y >> t.a.x >> t.a.y >> t.b.x >> t.b.y;
            std::cout << intersect(s, t) << '\n';
        }else if(type == "CONTAINS"){
            int n;
            std::cin >> n;
            std::vector<Point> polygon(static_cast<std::size_t>(n));
            for(auto& p: polygon) std::cin >> p.x >> p.y;
            Point p;
            std::cin >> p.x >> p.y;
            std::cout << contains(polygon, p) << '\n';
        }
    }
}
