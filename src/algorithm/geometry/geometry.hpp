#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <vector>

constexpr long double GEOMETRY_EPS = 1e-10L;

inline int geometry_sign(long double x){
    if(x > GEOMETRY_EPS) return 1;
    if(x < -GEOMETRY_EPS) return -1;
    return 0;
}

struct Point{
    long double x;
    long double y;

    Point() : x(0), y(0){}
    Point(long double x_, long double y_) : x(x_), y(y_){}

    Point operator+() const{ return *this; }
    Point operator-() const{ return {-x, -y}; }
    Point operator+(const Point& other) const{ return {x + other.x, y + other.y}; }
    Point operator-(const Point& other) const{ return {x - other.x, y - other.y}; }
    Point operator*(long double k) const{ return {x * k, y * k}; }
    Point operator/(long double k) const{ return {x / k, y / k}; }

    Point& operator+=(const Point& other){
        x += other.x;
        y += other.y;
        return *this;
    }

    Point& operator-=(const Point& other){
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Point& operator*=(long double k){
        x *= k;
        y *= k;
        return *this;
    }

    Point& operator/=(long double k){
        x /= k;
        y /= k;
        return *this;
    }

    bool operator<(const Point& other) const{
        if(geometry_sign(x - other.x) != 0) return x < other.x;
        return y < other.y - GEOMETRY_EPS;
    }

    bool operator==(const Point& other) const{
        return geometry_sign(x - other.x) == 0 && geometry_sign(y - other.y) == 0;
    }
};

inline Point operator*(long double k, const Point& p){
    return p * k;
}

struct Line{
    Point a;
    Point b;
};

using Segment = Line;

struct Circle{
    Point center;
    long double radius;
};

inline long double dot(const Point& a, const Point& b){
    return a.x * b.x + a.y * b.y;
}

inline long double cross(const Point& a, const Point& b){
    return a.x * b.y - a.y * b.x;
}

inline long double norm(const Point& p){
    return dot(p, p);
}

inline long double abs(const Point& p){
    return std::sqrt(norm(p));
}

inline long double arg(const Point& p){
    return std::atan2(p.y, p.x);
}

inline Point unit(const Point& p){
    return p / abs(p);
}

inline Point rotate90(const Point& p){
    return {-p.y, p.x};
}

inline Point rotate(const Point& p, long double theta){
    long double c = std::cos(theta);
    long double s = std::sin(theta);
    return {p.x * c - p.y * s, p.x * s + p.y * c};
}

inline Point polar(long double r, long double theta){
    return {r * std::cos(theta), r * std::sin(theta)};
}

enum CcwResult{
    CLOCKWISE = -1,
    COUNTER_CLOCKWISE = 1,
    ONLINE_BACK = 2,
    ONLINE_FRONT = -2,
    ON_SEGMENT = 0,
};

inline int ccw(Point a, Point b, Point c){
    b -= a;
    c -= a;
    int cr = geometry_sign(cross(b, c));
    if(cr > 0) return COUNTER_CLOCKWISE;
    if(cr < 0) return CLOCKWISE;
    if(geometry_sign(dot(b, c)) < 0) return ONLINE_BACK;
    if(geometry_sign(norm(b) - norm(c)) < 0) return ONLINE_FRONT;
    return ON_SEGMENT;
}

inline bool orthogonal(const Point& a, const Point& b){
    return geometry_sign(dot(a, b)) == 0;
}

inline bool parallel(const Point& a, const Point& b){
    return geometry_sign(cross(a, b)) == 0;
}

inline bool orthogonal(const Line& a, const Line& b){
    return orthogonal(a.b - a.a, b.b - b.a);
}

inline bool parallel(const Line& a, const Line& b){
    return parallel(a.b - a.a, b.b - b.a);
}

inline Point projection(const Line& line, const Point& p){
    Point base = line.b - line.a;
    return line.a + base * (dot(p - line.a, base) / norm(base));
}

inline Point reflection(const Line& line, const Point& p){
    return p + (projection(line, p) - p) * 2.0L;
}

inline bool intersect(const Segment& s, const Segment& t){
    return ccw(s.a, s.b, t.a) * ccw(s.a, s.b, t.b) <= 0 &&
           ccw(t.a, t.b, s.a) * ccw(t.a, t.b, s.b) <= 0;
}

inline bool intersect(const Line& line, const Point& p){
    return geometry_sign(cross(line.b - line.a, p - line.a)) == 0;
}

inline bool same_line(const Line& a, const Line& b){
    return parallel(a, b) && intersect(a, b.a);
}

inline bool intersect_line_line(const Line& a, const Line& b){
    return !parallel(a, b) || same_line(a, b);
}

inline bool intersect_line_segment(const Line& line, const Segment& segment){
    return geometry_sign(cross(line.b - line.a, segment.a - line.a)) *
           geometry_sign(cross(line.b - line.a, segment.b - line.a)) <= 0;
}

inline long double distance(const Point& a, const Point& b){
    return abs(a - b);
}

inline long double distance(const Line& line, const Point& p){
    return std::abs(cross(line.b - line.a, p - line.a)) / abs(line.b - line.a);
}

inline long double distance_segment_point(const Segment& segment, const Point& p){
    if(geometry_sign(dot(segment.b - segment.a, p - segment.a)) < 0) return abs(p - segment.a);
    if(geometry_sign(dot(segment.a - segment.b, p - segment.b)) < 0) return abs(p - segment.b);
    return distance(segment, p);
}

inline long double distance(const Segment& s, const Segment& t){
    if(intersect(s, t)) return 0.0L;
    return std::min({
        distance_segment_point(s, t.a),
        distance_segment_point(s, t.b),
        distance_segment_point(t, s.a),
        distance_segment_point(t, s.b),
    });
}

inline long double distance_line_line(const Line& a, const Line& b){
    if(intersect_line_line(a, b)) return 0.0L;
    return distance(a, b.a);
}

inline long double distance_line_segment(const Line& line, const Segment& segment){
    if(intersect_line_segment(line, segment)) return 0.0L;
    return std::min(distance(line, segment.a), distance(line, segment.b));
}

inline Point cross_point(const Line& a, const Line& b){
    long double d1 = std::abs(cross(b.b - b.a, a.a - b.a));
    long double d2 = std::abs(cross(b.b - b.a, a.b - b.a));
    return a.a + (a.b - a.a) * (d1 / (d1 + d2));
}

inline Point line_intersection(const Line& a, const Line& b){
    return cross_point(a, b);
}

inline Point segment_intersection(const Segment& a, const Segment& b){
    return cross_point(a, b);
}

inline long double area(const std::vector<Point>& polygon){
    long double result = 0;
    int n = static_cast<int>(polygon.size());
    for(int i = 0; i < n; i++){
        result += cross(polygon[static_cast<std::size_t>(i)], polygon[static_cast<std::size_t>((i + 1) % n)]);
    }
    return result / 2.0L;
}

inline int contains(const std::vector<Point>& polygon, const Point& p){
    bool inside = false;
    int n = static_cast<int>(polygon.size());
    for(int i = 0; i < n; i++){
        Point a = polygon[static_cast<std::size_t>(i)] - p;
        Point b = polygon[static_cast<std::size_t>((i + 1) % n)] - p;
        if(geometry_sign(cross(a, b)) == 0 && geometry_sign(dot(a, b)) <= 0) return 1;
        if(a.y > b.y) std::swap(a, b);
        if(geometry_sign(a.y) <= 0 && geometry_sign(b.y) > 0 && geometry_sign(cross(a, b)) > 0){
            inside = !inside;
        }
    }
    return inside ? 2 : 0;
}

inline int contains_convex(const std::vector<Point>& polygon, const Point& p){
    int n = static_cast<int>(polygon.size());
    if(n == 0) return 0;
    if(n == 1) return polygon[0] == p ? 1 : 0;
    if(n == 2) return ccw(polygon[0], polygon[1], p) == ON_SEGMENT ? 1 : 0;
    if(ccw(polygon[0], polygon[1], p) == ON_SEGMENT) return 1;
    if(ccw(polygon[0], polygon[static_cast<std::size_t>(n - 1)], p) == ON_SEGMENT) return 1;
    if(geometry_sign(cross(polygon[1] - polygon[0], p - polygon[0])) < 0) return 0;
    if(geometry_sign(cross(polygon[static_cast<std::size_t>(n - 1)] - polygon[0], p - polygon[0])) > 0) return 0;
    int low = 1;
    int high = n - 1;
    while(high - low > 1){
        int mid = (low + high) / 2;
        if(geometry_sign(cross(polygon[static_cast<std::size_t>(mid)] - polygon[0], p - polygon[0])) >= 0){
            low = mid;
        }else{
            high = mid;
        }
    }
    int c = ccw(polygon[static_cast<std::size_t>(low)], polygon[static_cast<std::size_t>(low + 1)], p);
    if(c == ON_SEGMENT) return 1;
    return c == COUNTER_CLOCKWISE ? 2 : 0;
}

inline bool is_convex(const std::vector<Point>& polygon){
    int n = static_cast<int>(polygon.size());
    for(int i = 0; i < n; i++){
        if(ccw(
               polygon[static_cast<std::size_t>(i)],
               polygon[static_cast<std::size_t>((i + 1) % n)],
               polygon[static_cast<std::size_t>((i + 2) % n)]
           ) == CLOCKWISE){
            return false;
        }
    }
    return true;
}

inline std::vector<Point> convex_hull(std::vector<Point> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    int n = static_cast<int>(points.size());
    if(n <= 1) return points;
    std::vector<Point> hull(static_cast<std::size_t>(2 * n));
    int k = 0;
    for(int i = 0; i < n; i++){
        while(k >= 2 && geometry_sign(cross(hull[static_cast<std::size_t>(k - 1)] - hull[static_cast<std::size_t>(k - 2)], points[static_cast<std::size_t>(i)] - hull[static_cast<std::size_t>(k - 1)])) <= 0){
            k--;
        }
        hull[static_cast<std::size_t>(k++)] = points[static_cast<std::size_t>(i)];
    }
    for(int i = n - 2, t = k + 1; i >= 0; i--){
        while(k >= t && geometry_sign(cross(hull[static_cast<std::size_t>(k - 1)] - hull[static_cast<std::size_t>(k - 2)], points[static_cast<std::size_t>(i)] - hull[static_cast<std::size_t>(k - 1)])) <= 0){
            k--;
        }
        hull[static_cast<std::size_t>(k++)] = points[static_cast<std::size_t>(i)];
    }
    hull.resize(static_cast<std::size_t>(k - 1));
    return hull;
}

inline long double convex_diameter(const std::vector<Point>& convex_polygon){
    int n = static_cast<int>(convex_polygon.size());
    if(n <= 1) return 0.0L;
    if(n == 2) return abs(convex_polygon[0] - convex_polygon[1]);
    int i = 0;
    int j = 0;
    for(int k = 1; k < n; k++){
        if(convex_polygon[static_cast<std::size_t>(k)].y < convex_polygon[static_cast<std::size_t>(i)].y) i = k;
        if(convex_polygon[static_cast<std::size_t>(k)].y > convex_polygon[static_cast<std::size_t>(j)].y) j = k;
    }
    long double result = 0;
    int si = i;
    int sj = j;
    do{
        result = std::max(result, abs(convex_polygon[static_cast<std::size_t>(i)] - convex_polygon[static_cast<std::size_t>(j)]));
        Point ni = convex_polygon[static_cast<std::size_t>((i + 1) % n)] - convex_polygon[static_cast<std::size_t>(i)];
        Point nj = convex_polygon[static_cast<std::size_t>((j + 1) % n)] - convex_polygon[static_cast<std::size_t>(j)];
        if(geometry_sign(cross(ni, nj)) >= 0){
            j = (j + 1) % n;
        }else{
            i = (i + 1) % n;
        }
    }while(i != si || j != sj);
    return result;
}

inline std::vector<Point> convex_cut(const std::vector<Point>& polygon, const Line& line){
    std::vector<Point> result;
    int n = static_cast<int>(polygon.size());
    for(int i = 0; i < n; i++){
        Point current = polygon[static_cast<std::size_t>(i)];
        Point next = polygon[static_cast<std::size_t>((i + 1) % n)];
        int c1 = geometry_sign(cross(line.b - line.a, current - line.a));
        int c2 = geometry_sign(cross(line.b - line.a, next - line.a));
        if(c1 >= 0) result.push_back(current);
        if(c1 * c2 < 0){
            result.push_back(cross_point({current, next}, line));
        }
    }
    return result;
}

inline int circle_relation(const Circle& a, const Circle& b){
    long double d = distance(a.center, b.center);
    long double rsum = a.radius + b.radius;
    long double rdiff = std::abs(a.radius - b.radius);
    if(geometry_sign(d - rsum) > 0) return 4;
    if(geometry_sign(d - rsum) == 0) return 3;
    if(geometry_sign(d - rdiff) == 0) return 1;
    if(geometry_sign(d - rdiff) < 0) return 0;
    return 2;
}

inline bool intersect_circle_line(const Circle& circle, const Line& line){
    return geometry_sign(distance(line, circle.center) - circle.radius) <= 0;
}

inline bool intersect_circle_segment(const Circle& circle, const Segment& segment){
    long double min_distance = distance_segment_point(segment, circle.center);
    long double max_distance = std::max(distance(circle.center, segment.a), distance(circle.center, segment.b));
    return geometry_sign(min_distance - circle.radius) <= 0 && geometry_sign(circle.radius - max_distance) <= 0;
}

inline bool intersect_circle_circle(const Circle& a, const Circle& b){
    int relation = circle_relation(a, b);
    return 1 <= relation && relation <= 3;
}

inline std::vector<Point> circle_line_cross_points(const Circle& circle, const Line& line){
    Point p = projection(line, circle.center);
    long double h2 = circle.radius * circle.radius - norm(p - circle.center);
    if(geometry_sign(h2) < 0) return {};
    if(geometry_sign(h2) == 0) return {p};
    Point e = unit(line.b - line.a);
    long double h = std::sqrt(std::max<long double>(0, h2));
    std::vector<Point> result = {p - e * h, p + e * h};
    std::sort(result.begin(), result.end());
    return result;
}

inline std::vector<Point> circle_segment_cross_points(const Circle& circle, const Segment& segment){
    std::vector<Point> result;
    for(const Point& p: circle_line_cross_points(circle, segment)){
        if(ccw(segment.a, segment.b, p) == ON_SEGMENT) result.push_back(p);
    }
    return result;
}

inline std::vector<Point> circle_circle_cross_points(const Circle& a, const Circle& b){
    long double d = distance(a.center, b.center);
    if(geometry_sign(d) == 0) return {};
    long double x = (a.radius * a.radius - b.radius * b.radius + d * d) / (2.0L * d);
    long double h2 = a.radius * a.radius - x * x;
    if(geometry_sign(h2) < 0) return {};
    Point e = (b.center - a.center) / d;
    Point p = a.center + e * x;
    if(geometry_sign(h2) == 0) return {p};
    long double h = std::sqrt(std::max<long double>(0, h2));
    std::vector<Point> result = {p - rotate90(e) * h, p + rotate90(e) * h};
    std::sort(result.begin(), result.end());
    return result;
}

inline std::vector<Point> tangent_points(const Circle& circle, const Point& p){
    Point v = p - circle.center;
    long double d2 = norm(v);
    long double r2 = circle.radius * circle.radius;
    if(geometry_sign(d2 - r2) < 0) return {};
    if(geometry_sign(d2 - r2) == 0) return {p};
    Point base = circle.center + v * (r2 / d2);
    Point offset = rotate90(v) * (circle.radius * std::sqrt(std::max<long double>(0, d2 - r2)) / d2);
    std::vector<Point> result = {base - offset, base + offset};
    std::sort(result.begin(), result.end());
    return result;
}

inline std::vector<Line> tangent_lines(const Circle& circle, const Point& p){
    std::vector<Line> result;
    for(const Point& q: tangent_points(circle, p)){
        result.push_back({p, q});
    }
    return result;
}

inline std::vector<Line> common_tangents(const Circle& a, const Circle& b){
    std::vector<Line> result;
    Point v = b.center - a.center;
    long double d2 = norm(v);
    if(geometry_sign(d2) == 0) return result;
    for(int same_side: {-1, 1}){
        long double r = a.radius - same_side * b.radius;
        long double h2 = d2 - r * r;
        if(geometry_sign(h2) < 0) continue;
        long double h = std::sqrt(std::max<long double>(0, h2));
        for(int sign: {-1, 1}){
            Point n = (v * r + rotate90(v) * (h * sign)) / d2;
            Point p1 = a.center + n * a.radius;
            Point p2 = b.center + n * (same_side * b.radius);
            result.push_back({p1, p2});
            if(geometry_sign(h) == 0) break;
        }
    }
    return result;
}

inline Point circumcenter(const Point& a, const Point& b, const Point& c){
    Point p = (a + b) / 2.0L;
    Point q = (a + c) / 2.0L;
    Line u{p, p + rotate90(b - a)};
    Line v{q, q + rotate90(c - a)};
    return cross_point(u, v);
}

inline Circle circumcircle(const Point& a, const Point& b, const Point& c){
    Point center = circumcenter(a, b, c);
    return {center, distance(center, a)};
}

inline Point incenter(const Point& a, const Point& b, const Point& c){
    long double x = distance(b, c);
    long double y = distance(c, a);
    long double z = distance(a, b);
    return (a * x + b * y + c * z) / (x + y + z);
}

inline Circle incircle(const Point& a, const Point& b, const Point& c){
    Point center = incenter(a, b, c);
    return {center, distance(Line{a, b}, center)};
}

inline long double closest_pair(std::vector<Point> points){
    int n = static_cast<int>(points.size());
    if(n < 2) return std::numeric_limits<long double>::infinity();
    std::sort(points.begin(), points.end(), [](const Point& a, const Point& b){
        if(geometry_sign(a.x - b.x) != 0) return a.x < b.x;
        return a.y < b.y;
    });
    std::vector<Point> buffer(static_cast<std::size_t>(n));
    auto solve = [&](auto&& self, int l, int r) -> long double {
        if(r - l <= 1) return std::numeric_limits<long double>::infinity();
        int m = (l + r) / 2;
        long double x = points[static_cast<std::size_t>(m)].x;
        long double d = std::min(self(self, l, m), self(self, m, r));
        std::inplace_merge(
            points.begin() + l,
            points.begin() + m,
            points.begin() + r,
            [](const Point& a, const Point& b){
                if(geometry_sign(a.y - b.y) != 0) return a.y < b.y;
                return a.x < b.x;
            }
        );
        int count = 0;
        for(int i = l; i < r; i++){
            if(std::abs(points[static_cast<std::size_t>(i)].x - x) >= d) continue;
            for(int j = count - 1; j >= 0; j--){
                Point diff = points[static_cast<std::size_t>(i)] - buffer[static_cast<std::size_t>(j)];
                if(diff.y >= d) break;
                d = std::min(d, abs(diff));
            }
            buffer[static_cast<std::size_t>(count++)] = points[static_cast<std::size_t>(i)];
        }
        return d;
    };
    return solve(solve, 0, n);
}