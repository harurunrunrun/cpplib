// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "../../src/algorithm/geometry/2d/geometry.hpp"

void self_test(){
    auto close = [](long double a, long double b){
        return std::abs(a - b) <= 1e-8L;
    };
    auto expect_invalid = [](auto&& operation){
        bool thrown = false;
        try{
            operation();
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    };
    auto expect_domain = [](auto&& operation){
        bool thrown = false;
        try{
            operation();
        }catch(const std::domain_error&){
            thrown = true;
        }
        assert(thrown);
    };

    assert(ccw({0, 0}, {2, 0}, {1, 1}) == COUNTER_CLOCKWISE);
    assert(ccw({0, 0}, {2, 0}, {1, -1}) == CLOCKWISE);
    assert(ccw({0, 0}, {2, 0}, {-1, 0}) == ONLINE_BACK);
    assert(ccw({0, 0}, {2, 0}, {3, 0}) == ONLINE_FRONT);
    assert(ccw({0, 0}, {2, 0}, {1, 0}) == ON_SEGMENT);
    assert(intersect(Segment{{0, 0}, {2, 2}}, Segment{{0, 2}, {2, 0}}));
    assert(!intersect(Segment{{0, 0}, {1, 0}}, Segment{{2, 0}, {3, 0}}));
    assert(!intersect(Segment{{0, 0}, {0, 0}}, Segment{{1, 0}, {1, 0}}));
    assert(intersect(Segment{{0, 0}, {0, 0}}, Segment{{0, 0}, {0, 0}}));
    assert(on_segment({{1, 1}, {1, 1}}, {1, 1}));
    assert(!on_segment({{1, 1}, {1, 1}}, {2, 1}));

    Point projected = projection(Line{{0, 0}, {2, 0}}, {1, 3});
    assert(projected == Point(1, 0));
    assert(reflection(Line{{0, 0}, {2, 0}}, {1, 3}) == Point(1, -3));
    Point outside_cross = line_intersection({{0, 0}, {1, 0}}, {{2, -1}, {2, 1}});
    assert(outside_cross == Point(2, 0));
    assert(segment_intersection({{0, 0}, {2, 0}}, {{2, 0}, {3, 0}}) == Point(2, 0));
    expect_domain([]{ (void)line_intersection({{0, 0}, {1, 0}}, {{0, 1}, {1, 1}}); });
    expect_domain([]{ (void)segment_intersection({{0, 0}, {2, 0}}, {{1, 0}, {3, 0}}); });
    expect_invalid([]{ (void)projection({{0, 0}, {0, 0}}, {1, 1}); });
    expect_invalid([]{ (void)unit({0, 0}); });
    assert(close(distance_segment_point({{1, 2}, {1, 2}}, {4, 6}), 5.0L));

    std::vector<Point> polygon = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    std::vector<Point> reversed = polygon;
    std::reverse(reversed.begin(), reversed.end());
    assert(contains(polygon, {2, 2}) == 2);
    assert(contains(polygon, {4, 2}) == 1);
    assert(contains(polygon, {5, 2}) == 0);
    assert(contains_convex(polygon, {2, 2}) == 2);
    assert(contains_convex(reversed, {2, 2}) == 2);
    assert(contains_convex(reversed, {4, 2}) == 1);
    assert(contains_convex(reversed, {5, 2}) == 0);
    assert(is_convex(polygon));
    assert(is_convex(reversed));
    std::vector<Point> weak_convex = {
        {0, 0}, {0, 0}, {2, 0}, {4, 0}, {4, 2}, {4, 4}, {0, 4}, {0, 2}
    };
    ConvexPolygonQuery weak_query(weak_convex);
    assert(weak_query.size() == 4);
    assert(weak_query.contains({2, 2}) == 2);
    assert(weak_query.contains({3, 0}) == 1);
    assert(weak_query.contains({5, 2}) == 0);
    ConvexPolygonQuery line_query({{2, 0}, {0, 0}, {1, 0}, {2, 0}});
    assert(line_query.contains({1, 0}) == 1 && line_query.contains({1, 1}) == 0);
    for(int x = -1; x <= 5; x++){
        for(int y = -1; y <= 5; y++){
            Point point{static_cast<long double>(x), static_cast<long double>(y)};
            assert(contains_convex(weak_convex, point) == contains(weak_convex, point));
        }
    }
    std::reverse(weak_convex.begin(), weak_convex.end());
    for(int x = -1; x <= 5; x++){
        for(int y = -1; y <= 5; y++){
            Point point{static_cast<long double>(x), static_cast<long double>(y)};
            assert(contains_convex(weak_convex, point) == contains(weak_convex, point));
        }
    }
    assert(!is_convex({{0, 0}, {2, 0}, {1, 1}, {2, 2}, {0, 2}}));
    auto hull = convex_hull({{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0.5L, 0.5L}, {1, 1}});
    assert(hull.size() == 4);
    assert(close(area(hull), 1.0L));
    assert(close(convex_diameter(polygon), std::sqrt(32.0L)));
    auto cut = convex_cut(polygon, {{2, -1}, {2, 5}});
    assert(close(std::abs(area(cut)), 8.0L));
    for(const Point& point: cut){
        assert(geometry_sign(cross(Point{0, 6}, point - Point{2, -1})) >= 0);
    }

    Circle c{{0, 0}, 5};
    auto line_points = circle_line_cross_points(c, Line{{-10, 0}, {10, 0}});
    assert(line_points.size() == 2);
    assert(line_points[0] == Point(-5, 0));
    assert(line_points[1] == Point(5, 0));
    auto segment_points = circle_segment_cross_points(c, Segment{{0, 0}, {10, 0}});
    assert(segment_points.size() == 1 && segment_points[0] == Point(5, 0));
    auto point_segment = circle_segment_cross_points(c, Segment{{5, 0}, {5, 0}});
    assert(point_segment.size() == 1 && point_segment[0] == Point(5, 0));
    assert(circle_relation(Circle{{0, 0}, 2}, Circle{{5, 0}, 1}) == CIRCLE_SEPARATE);
    assert(circle_relation(Circle{{0, 0}, 2}, Circle{{3, 0}, 1}) == CIRCLE_EXTERNALLY_TANGENT);
    assert(circle_relation(Circle{{0, 0}, 2}, Circle{{2, 0}, 2}) == CIRCLE_INTERSECT);
    assert(circle_relation(Circle{{0, 0}, 3}, Circle{{2, 0}, 1}) == CIRCLE_INTERNALLY_TANGENT);
    assert(circle_relation(Circle{{0, 0}, 4}, Circle{{1, 0}, 1}) == CIRCLE_INSIDE);
    assert(circle_relation(Circle{{1, 2}, 3}, Circle{{1, 2}, 3}) == CIRCLE_COINCIDENT);
    assert(intersect_circle_circle(Circle{{1, 2}, 3}, Circle{{1, 2}, 3}));
    expect_domain([]{ (void)circle_circle_cross_points(Circle{{0, 0}, 1}, Circle{{0, 0}, 1}); });
    expect_domain([]{ (void)common_tangents(Circle{{0, 0}, 1}, Circle{{0, 0}, 1}); });
    expect_invalid([]{ (void)circle_relation(Circle{{0, 0}, -1}, Circle{{1, 0}, 1}); });
    const long double pi = std::acos(-1.0L);
    assert(close(
        circle_circle_intersection_area(Circle{{0, 0}, 1}, Circle{{3, 0}, 1}),
        0.0L
    ));
    assert(close(
        circle_circle_intersection_area(Circle{{0, 0}, 1}, Circle{{2, 0}, 1}),
        0.0L
    ));
    assert(close(
        circle_circle_intersection_area(Circle{{0, 0}, 3}, Circle{{1, 0}, 1}),
        pi
    ));
    assert(close(
        circle_circle_intersection_area(Circle{{0, 0}, 2}, Circle{{1, 0}, 1}),
        pi
    ));
    assert(close(
        circle_circle_intersection_area(Circle{{1, 2}, 3}, Circle{{1, 2}, 3}),
        9.0L * pi
    ));
    assert(close(
        circle_circle_intersection_area(Circle{{0, 0}, 0}, Circle{{0, 0}, 0}),
        0.0L
    ));
    const long double equal_overlap = 2.0L * pi / 3.0L - std::sqrt(3.0L) / 2.0L;
    assert(close(
        circle_circle_intersection_area(Circle{{0, 0}, 1}, Circle{{1, 0}, 1}),
        equal_overlap
    ));
    assert(close(
        circle_circle_intersection_area(Circle{{0, 0}, 1}, Circle{{2, 0}, 2}),
        1.40306643968573875104L
    ));
    assert(close(
        circle_circle_intersection_area(
            Circle{{1e12L, -1e12L}, 1},
            Circle{{1e12L + 1, -1e12L}, 1}
        ),
        equal_overlap
    ));
    const long double almost_tangent = circle_circle_intersection_area(
        Circle{{0, 0}, 1},
        Circle{{2.0L - 1e-12L, 0}, 1}
    );
    assert(0.0L < almost_tangent && almost_tangent < 1e-15L);
    const Circle first_overlap{{0, 0}, 5};
    const Circle second_overlap{{8, 0}, 5};
    assert(close(
        circle_circle_intersection_area(first_overlap, second_overlap),
        circle_circle_intersection_area(second_overlap, first_overlap)
    ));
    expect_invalid([]{
        (void)circle_circle_intersection_area(Circle{{0, 0}, -1}, Circle{{1, 0}, 1});
    });
    expect_invalid([]{
        (void)circle_circle_intersection_area(Circle{{0, 0}, 1}, Circle{{1, 0}, -1});
    });
    auto cc = circle_circle_cross_points(Circle{{0, 0}, 5}, Circle{{8, 0}, 5});
    assert(cc.size() == 2);
    for(const Point& point: cc){
        assert(close(distance(point, Point{0, 0}), 5.0L));
        assert(close(distance(point, Point{8, 0}), 5.0L));
    }
    auto tangency = tangent_points(Circle{{0, 0}, 1}, {2, 0});
    assert(tangency.size() == 2);
    for(const Point& point: tangency){
        assert(close(norm(point), 1.0L));
        assert(close(dot(point, Point{2, 0} - point), 0.0L));
    }
    auto tangent_at_point = tangent_lines(Circle{{0, 0}, 1}, {1, 0});
    assert(tangent_at_point.size() == 1);
    assert(!close(abs(tangent_at_point[0].b - tangent_at_point[0].a), 0.0L));
    assert(close(distance(tangent_at_point[0], Point{0, 0}), 1.0L));
    auto touching_tangents = common_tangents(Circle{{0, 0}, 1}, Circle{{2, 0}, 1});
    assert(touching_tangents.size() == 3);
    for(const Line& line: touching_tangents){
        assert(!close(abs(line.b - line.a), 0.0L));
        assert(close(distance(line, Point{0, 0}), 1.0L));
        assert(close(distance(line, Point{2, 0}), 1.0L));
    }
    auto circum = circumcircle({0, 0}, {2, 0}, {0, 2});
    assert(circum.center == Point(1, 1));
    assert(close(circum.radius, std::sqrt(2.0L)));
    auto in = incircle({0, 0}, {2, 0}, {0, 2});
    assert(close(distance(Line{{0, 0}, {2, 0}}, in.center), in.radius));
    expect_invalid([]{ (void)circumcenter({0, 0}, {1, 0}, {2, 0}); });
    assert(close(closest_pair({{0, 0}, {10, 0}, {3, 4}, {3, 5}}), 1.0L));

    auto exact_on_segment = [](long long ax, long long ay, long long bx, long long by, long long px, long long py){
        long long cr = (bx - ax) * (py - ay) - (by - ay) * (px - ax);
        long long dt = (px - ax) * (px - bx) + (py - ay) * (py - by);
        return cr == 0 && dt <= 0;
    };
    auto exact_intersect = [&](const Segment& a, const Segment& b){
        auto coordinate = [](long double value){ return static_cast<long long>(std::llround(value)); };
        long long ax = coordinate(a.a.x), ay = coordinate(a.a.y);
        long long bx = coordinate(a.b.x), by = coordinate(a.b.y);
        long long cx = coordinate(b.a.x), cy = coordinate(b.a.y);
        long long dx = coordinate(b.b.x), dy = coordinate(b.b.y);
        auto orient = [](long long x1, long long y1, long long x2, long long y2, long long x3, long long y3){
            long long value = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
            return (value > 0) - (value < 0);
        };
        int ab_c = orient(ax, ay, bx, by, cx, cy);
        int ab_d = orient(ax, ay, bx, by, dx, dy);
        int cd_a = orient(cx, cy, dx, dy, ax, ay);
        int cd_b = orient(cx, cy, dx, dy, bx, by);
        if(ab_c == 0 && exact_on_segment(ax, ay, bx, by, cx, cy)) return true;
        if(ab_d == 0 && exact_on_segment(ax, ay, bx, by, dx, dy)) return true;
        if(cd_a == 0 && exact_on_segment(cx, cy, dx, dy, ax, ay)) return true;
        if(cd_b == 0 && exact_on_segment(cx, cy, dx, dy, bx, by)) return true;
        return ab_c * ab_d < 0 && cd_a * cd_b < 0;
    };

    std::mt19937 rng(712367U);
    std::uniform_int_distribution<int> coordinate(-20, 20);
    auto random_point = [&]{
        return Point(
            static_cast<long double>(coordinate(rng)),
            static_cast<long double>(coordinate(rng))
        );
    };
    for(int iteration = 0; iteration < 500; iteration++){
        Segment a{random_point(), random_point()};
        Segment b{random_point(), random_point()};
        assert(intersect(a, b) == exact_intersect(a, b));
    }
    for(int iteration = 0; iteration < 200; iteration++){
        Line line{random_point(), random_point()};
        if(line.a == line.b){
            iteration--;
            continue;
        }
        Point point = random_point();
        Point foot = projection(line, point);
        Point reflected = reflection(line, point);
        assert(close(cross(line.b - line.a, foot - line.a), 0.0L));
        assert(close(dot(line.b - line.a, point - foot), 0.0L));
        assert(close((point.x + reflected.x) / 2.0L, foot.x));
        assert(close((point.y + reflected.y) / 2.0L, foot.y));
    }
    for(int iteration = 0; iteration < 200; iteration++){
        std::vector<Point> points;
        for(int i = 0; i < 20; i++) points.push_back(random_point());
        auto random_hull = convex_hull(points);
        long double expected_diameter = 0.0L;
        for(const Point& a: random_hull){
            for(const Point& b: random_hull) expected_diameter = std::max(expected_diameter, distance(a, b));
        }
        assert(close(convex_diameter(random_hull), expected_diameter));
        std::vector<Point> reverse_hull = random_hull;
        std::reverse(reverse_hull.begin(), reverse_hull.end());
        assert(close(convex_diameter(reverse_hull), expected_diameter));
        std::vector<Point> weak_hull;
        for(std::size_t i = 0; i < random_hull.size(); i++){
            const Point& a = random_hull[i];
            const Point& b = random_hull[(i + 1) % random_hull.size()];
            weak_hull.push_back(a);
            weak_hull.push_back((a + b) / 2.0L);
            if(i == 1) weak_hull.push_back((a + b) / 2.0L);
        }
        assert(close(convex_diameter(weak_hull), expected_diameter));
        std::vector<Point> reverse_weak_hull = weak_hull;
        std::reverse(reverse_weak_hull.begin(), reverse_weak_hull.end());
        ConvexPolygonQuery hull_query(random_hull);
        ConvexPolygonQuery reverse_hull_query(reverse_hull);
        ConvexPolygonQuery weak_hull_query(weak_hull);
        ConvexPolygonQuery reverse_weak_hull_query(reverse_weak_hull);
        assert(close(convex_diameter(reverse_weak_hull), expected_diameter));
        for(int query = 0; query < 20; query++){
            Point point = random_point();
            int expected = contains(random_hull, point);
            assert(contains_convex(random_hull, point) == expected);
            assert(contains_convex(reverse_hull, point) == expected);
            assert(contains_convex(weak_hull, point) == expected);
            assert(contains_convex(reverse_weak_hull, point) == expected);
            assert(hull_query.contains(point) == expected);
            assert(reverse_hull_query.contains(point) == expected);
            assert(weak_hull_query.contains(point) == expected);
            assert(reverse_weak_hull_query.contains(point) == expected);
        }
        long double expected_closest = std::numeric_limits<long double>::infinity();
        for(std::size_t i = 0; i < points.size(); i++){
            for(std::size_t j = i + 1; j < points.size(); j++){
                expected_closest = std::min(expected_closest, distance(points[i], points[j]));
            }
        }
        assert(close(closest_pair(points), expected_closest));
    }
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
        }else if(type == "LINE_INTERSECTION"){
            Line a, b;
            std::cin >> a.a.x >> a.a.y >> a.b.x >> a.b.y >> b.a.x >> b.a.y >> b.b.x >> b.b.y;
            Point p = line_intersection(a, b);
            std::cout << std::llround(p.x) << ' ' << std::llround(p.y) << '\n';
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
        }else if(type == "CONVEX_QUERY"){
            int n, m;
            std::cin >> n >> m;
            std::vector<Point> polygon(static_cast<std::size_t>(n));
            for(auto& vertex: polygon) std::cin >> vertex.x >> vertex.y;
            ConvexPolygonQuery query(std::move(polygon));
            std::cout << m;
            while(m--){
                Point point;
                std::cin >> point.x >> point.y;
                std::cout << ' ' << query.contains(point);
            }
            std::cout << '\n';
        }else if(type == "IS_CONVEX"){
            int n;
            std::cin >> n;
            std::vector<Point> polygon(static_cast<std::size_t>(n));
            for(auto& p: polygon) std::cin >> p.x >> p.y;
            std::cout << is_convex(polygon) << '\n';
        }else if(type == "CONVEX_DIAMETER2"){
            int n;
            std::cin >> n;
            std::vector<Point> polygon(static_cast<std::size_t>(n));
            for(auto& p: polygon) std::cin >> p.x >> p.y;
            long double diameter = convex_diameter(polygon);
            std::cout << std::llround(diameter * diameter) << '\n';
        }else if(type == "CLOSEST_PAIR2"){
            int n;
            std::cin >> n;
            std::vector<Point> points(static_cast<std::size_t>(n));
            for(auto& p: points) std::cin >> p.x >> p.y;
            long double result = closest_pair(points);
            std::cout << std::llround(result * result) << '\n';
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
            try{
                std::cout << circle_circle_cross_points(a, b).size() << '\n';
            }catch(const std::domain_error&){
                std::cout << -1 << '\n';
            }
        }else if(type == "CIRCLE_CIRCLE_AREA_1E9"){
            Circle a, b;
            std::cin >> a.center.x >> a.center.y >> a.radius >> b.center.x >> b.center.y >> b.radius;
            std::cout << std::llround(circle_circle_intersection_area(a, b) * 1e9L) << '\n';
        }else if(type == "COMMON_TANGENT_COUNT"){
            Circle a, b;
            std::cin >> a.center.x >> a.center.y >> a.radius >> b.center.x >> b.center.y >> b.radius;
            try{
                std::cout << common_tangents(a, b).size() << '\n';
            }catch(const std::domain_error&){
                std::cout << -1 << '\n';
            }
        }
    }
}
