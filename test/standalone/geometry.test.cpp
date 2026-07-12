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
    assert(contains_convex(polygon, {2, 2}) == 2);
    assert(contains_convex(polygon, {4, 2}) == 1);
    assert(contains_convex(polygon, {5, 2}) == 0);

    Circle c{{0, 0}, 5};
    auto line_points = circle_line_cross_points(c, Line{{-10, 0}, {10, 0}});
    assert(line_points.size() == 2);
    assert(line_points[0] == Point(-5, 0));
    assert(line_points[1] == Point(5, 0));
    auto segment_points = circle_segment_cross_points(c, Segment{{0, 0}, {10, 0}});
    assert(segment_points.size() == 1);
    assert(segment_points[0] == Point(5, 0));
    assert(circle_relation(Circle{{0, 0}, 2}, Circle{{5, 0}, 1}) == 4);
    assert(circle_relation(Circle{{0, 0}, 2}, Circle{{3, 0}, 1}) == 3);
    assert(circle_relation(Circle{{0, 0}, 2}, Circle{{2, 0}, 2}) == 2);
    assert(circle_relation(Circle{{0, 0}, 3}, Circle{{2, 0}, 1}) == 1);
    assert(circle_relation(Circle{{0, 0}, 4}, Circle{{1, 0}, 1}) == 0);
    auto cc = circle_circle_cross_points(Circle{{0, 0}, 5}, Circle{{8, 0}, 5});
    assert(cc.size() == 2);
    assert(geometry_sign(cc[0].x - 4) == 0);
    assert(geometry_sign(std::abs(cc[0].y) - 3) == 0);
    auto tangent = tangent_points(Circle{{0, 0}, 1}, {2, 0});
    assert(tangent.size() == 2);
    assert(geometry_sign(norm(tangent[0]) - 1) == 0);
    assert(geometry_sign(dot(tangent[0], Point{2, 0} - tangent[0])) == 0);
    assert(common_tangents(Circle{{0, 0}, 1}, Circle{{4, 0}, 1}).size() == 4);
    auto circum = circumcircle({0, 0}, {2, 0}, {0, 2});
    assert(circum.center == Point(1, 1));
    assert(geometry_sign(circum.radius - std::sqrt(2.0L)) == 0);
    auto in = incircle({0, 0}, {2, 0}, {0, 2});
    assert(geometry_sign(distance(Line{{0, 0}, {2, 0}}, in.center) - in.radius) == 0);
    assert(geometry_sign(closest_pair({{0, 0}, {10, 0}, {3, 4}, {3, 5}}) - 1.0L) == 0);
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
        }else if(type == "CONTAINS_CONVEX"){
            int n;
            std::cin >> n;
            std::vector<Point> polygon(static_cast<std::size_t>(n));
            for(auto& p: polygon) std::cin >> p.x >> p.y;
            Point p;
            std::cin >> p.x >> p.y;
            std::cout << contains_convex(polygon, p) << '\n';
        }else if(type == "CIRCLE_RELATION"){
            Circle a, b;
            std::cin >> a.center.x >> a.center.y >> a.radius >> b.center.x >> b.center.y >> b.radius;
            std::cout << circle_relation(a, b) << '\n';
        }else if(type == "CIRCLE_LINE_COUNT"){
            Circle c;
            Line l;
            std::cin >> c.center.x >> c.center.y >> c.radius >> l.a.x >> l.a.y >> l.b.x >> l.b.y;
            std::cout << circle_line_cross_points(c, l).size() << '\n';
        }else if(type == "CIRCLE_CIRCLE_COUNT"){
            Circle a, b;
            std::cin >> a.center.x >> a.center.y >> a.radius >> b.center.x >> b.center.y >> b.radius;
            std::cout << circle_circle_cross_points(a, b).size() << '\n';
        }else if(type == "COMMON_TANGENT_COUNT"){
            Circle a, b;
            std::cin >> a.center.x >> a.center.y >> a.radius >> b.center.x >> b.center.y >> b.radius;
            std::cout << common_tangents(a, b).size() << '\n';
        }
    }
}
