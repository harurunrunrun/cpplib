#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>
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
        if(x != other.x) return x < other.x;
        return y < other.y;
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
    long double length = abs(p);
    if(geometry_sign(length) == 0) throw std::invalid_argument("unit vector of zero vector");
    return p / length;
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
    if(geometry_sign(abs(a.b - a.a)) == 0 || geometry_sign(abs(b.b - b.a)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    return orthogonal(a.b - a.a, b.b - b.a);
}

inline bool parallel(const Line& a, const Line& b){
    if(geometry_sign(abs(a.b - a.a)) == 0 || geometry_sign(abs(b.b - b.a)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    return parallel(a.b - a.a, b.b - b.a);
}

inline Point projection(const Line& line, const Point& p){
    Point base = line.b - line.a;
    if(geometry_sign(abs(base)) == 0) throw std::invalid_argument("degenerate line");
    return line.a + base * (dot(p - line.a, base) / norm(base));
}

inline Point reflection(const Line& line, const Point& p){
    return p + (projection(line, p) - p) * 2.0L;
}

inline bool on_segment(const Segment& segment, const Point& p){
    return geometry_sign(cross(segment.b - segment.a, p - segment.a)) == 0 &&
           geometry_sign(dot(p - segment.a, p - segment.b)) <= 0;
}

inline bool intersect(const Segment& s, const Segment& t){
    int c1 = geometry_sign(cross(s.b - s.a, t.a - s.a));
    int c2 = geometry_sign(cross(s.b - s.a, t.b - s.a));
    int c3 = geometry_sign(cross(t.b - t.a, s.a - t.a));
    int c4 = geometry_sign(cross(t.b - t.a, s.b - t.a));
    if(c1 == 0 && on_segment(s, t.a)) return true;
    if(c2 == 0 && on_segment(s, t.b)) return true;
    if(c3 == 0 && on_segment(t, s.a)) return true;
    if(c4 == 0 && on_segment(t, s.b)) return true;
    return c1 * c2 < 0 && c3 * c4 < 0;
}

inline bool intersect(const Line& line, const Point& p){
    if(geometry_sign(abs(line.b - line.a)) == 0) throw std::invalid_argument("degenerate line");
    return geometry_sign(cross(line.b - line.a, p - line.a)) == 0;
}

inline bool same_line(const Line& a, const Line& b){
    return parallel(a, b) && intersect(a, b.a);
}

inline bool intersect_line_line(const Line& a, const Line& b){
    return !parallel(a, b) || same_line(a, b);
}

inline bool intersect_line_segment(const Line& line, const Segment& segment){
    if(geometry_sign(abs(line.b - line.a)) == 0) throw std::invalid_argument("degenerate line");
    return geometry_sign(cross(line.b - line.a, segment.a - line.a)) *
           geometry_sign(cross(line.b - line.a, segment.b - line.a)) <= 0;
}

inline long double distance(const Point& a, const Point& b){
    return abs(a - b);
}

inline long double distance(const Line& line, const Point& p){
    if(geometry_sign(abs(line.b - line.a)) == 0) throw std::invalid_argument("degenerate line");
    return std::abs(cross(line.b - line.a, p - line.a)) / abs(line.b - line.a);
}

inline long double distance_segment_point(const Segment& segment, const Point& p){
    if(geometry_sign(abs(segment.b - segment.a)) == 0) return abs(p - segment.a);
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
    Point da = a.b - a.a;
    Point db = b.b - b.a;
    if(geometry_sign(abs(da)) == 0 || geometry_sign(abs(db)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    long double denominator = cross(da, db);
    if(geometry_sign(denominator) == 0) throw std::domain_error("lines do not have a unique intersection");
    return a.a + da * (cross(b.a - a.a, db) / denominator);
}

inline Point line_intersection(const Line& a, const Line& b){
    return cross_point(a, b);
}

inline Point segment_intersection(const Segment& a, const Segment& b){
    if(!intersect(a, b)) throw std::domain_error("segments do not intersect");
    Point da = a.b - a.a;
    Point db = b.b - b.a;
    if(geometry_sign(abs(da)) != 0 && geometry_sign(abs(db)) != 0 &&
       geometry_sign(cross(da, db)) != 0){
        return cross_point(a, b);
    }
    std::vector<Point> candidates;
    for(const Point& p: {a.a, a.b, b.a, b.b}){
        if(on_segment(a, p) && on_segment(b, p) &&
           std::none_of(candidates.begin(), candidates.end(), [&](const Point& q){ return p == q; })){
            candidates.push_back(p);
        }
    }
    if(candidates.size() == 1) return candidates[0];
    throw std::domain_error("segments do not have a unique intersection");
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

class ConvexPolygonQuery{
    std::vector<Point> polygon_;

    static std::vector<Point> normalize(std::vector<Point> polygon);

public:
    explicit ConvexPolygonQuery(std::vector<Point> polygon);

    int size() const{ return static_cast<int>(polygon_.size()); }
    const std::vector<Point>& vertices() const{ return polygon_; }
    int contains(const Point& point) const;
};

inline std::vector<Point> ConvexPolygonQuery::normalize(std::vector<Point> polygon){
    std::size_t write = 0;
    for(const Point& point: polygon){
        if(write == 0 || !(polygon[write - 1] == point)){
            polygon[write++] = point;
        }
    }
    polygon.resize(write);
    if(polygon.size() >= 2 && polygon.front() == polygon.back()) polygon.pop_back();
    if(polygon.size() <= 2) return polygon;

    const int n = static_cast<int>(polygon.size());
    int orientation = 0;
    for(int i = 0; i < n; ++i){
        orientation = geometry_sign(cross(
            polygon[static_cast<std::size_t>((i + 1) % n)] - polygon[static_cast<std::size_t>(i)],
            polygon[static_cast<std::size_t>((i + 2) % n)] - polygon[static_cast<std::size_t>((i + 1) % n)]
        ));
        if(orientation != 0) break;
    }
    if(orientation == 0){
        const auto [minimum, maximum] = std::minmax_element(polygon.begin(), polygon.end());
        if(*minimum == *maximum) return {*minimum};
        return {*minimum, *maximum};
    }
    if(orientation < 0) std::reverse(polygon.begin(), polygon.end());

    int start = 0;
    for(int i = 0; i < n; ++i){
        const Point& previous = polygon[static_cast<std::size_t>((i + n - 1) % n)];
        const Point& current = polygon[static_cast<std::size_t>(i)];
        const Point& next = polygon[static_cast<std::size_t>((i + 1) % n)];
        if(geometry_sign(cross(current - previous, next - current)) > 0){
            start = i;
            break;
        }
    }

    std::vector<Point> result;
    result.reserve(polygon.size() + 1);
    for(int offset = 0; offset <= n; ++offset){
        const Point& point = polygon[static_cast<std::size_t>((start + offset) % n)];
        while(result.size() >= 2 && geometry_sign(cross(
            result.back() - result[result.size() - 2],
            point - result.back()
        )) == 0){
            result.pop_back();
        }
        result.push_back(point);
    }
    result.pop_back();
    return result;
}

inline ConvexPolygonQuery::ConvexPolygonQuery(std::vector<Point> polygon)
    : polygon_(normalize(std::move(polygon))){}

inline int ConvexPolygonQuery::contains(const Point& point) const{
    const int n = size();
    if(n == 0) return 0;
    if(n == 1) return polygon_[0] == point ? 1 : 0;
    if(n == 2) return on_segment({polygon_[0], polygon_[1]}, point) ? 1 : 0;

    const Point& origin = polygon_[0];
    const int first_side = geometry_sign(cross(polygon_[1] - origin, point - origin));
    const int last_side = geometry_sign(cross(
        polygon_[static_cast<std::size_t>(n - 1)] - origin,
        point - origin
    ));
    if(first_side < 0 || last_side > 0) return 0;
    if(first_side == 0) return on_segment({origin, polygon_[1]}, point) ? 1 : 0;
    if(last_side == 0){
        return on_segment({origin, polygon_[static_cast<std::size_t>(n - 1)]}, point) ? 1 : 0;
    }

    int low = 1;
    int high = n - 1;
    while(high - low > 1){
        const int middle = low + (high - low) / 2;
        if(geometry_sign(cross(
            polygon_[static_cast<std::size_t>(middle)] - origin,
            point - origin
        )) >= 0){
            low = middle;
        }else{
            high = middle;
        }
    }
    const int side = geometry_sign(cross(
        polygon_[static_cast<std::size_t>(low + 1)] - polygon_[static_cast<std::size_t>(low)],
        point - polygon_[static_cast<std::size_t>(low)]
    ));
    if(side == 0) return 1;
    return side > 0 ? 2 : 0;
}

inline int contains_convex(const std::vector<Point>& polygon, const Point& point){
    return ConvexPolygonQuery(polygon).contains(point);
}
inline bool is_convex(const std::vector<Point>& polygon){
    int n = static_cast<int>(polygon.size());
    int direction = 0;
    for(int i = 0; i < n; i++){
        int turn = geometry_sign(cross(
            polygon[static_cast<std::size_t>((i + 1) % n)] - polygon[static_cast<std::size_t>(i)],
            polygon[static_cast<std::size_t>((i + 2) % n)] - polygon[static_cast<std::size_t>((i + 1) % n)]
        ));
        if(turn == 0) continue;
        if(direction == 0) direction = turn;
        else if(direction != turn) return false;
    }
    return true;
}
inline std::vector<Point> convex_hull(std::vector<Point> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end(), [](const Point& a, const Point& b){
        return a.x == b.x && a.y == b.y;
    }), points.end());
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
    std::vector<Point> vertices;
    vertices.reserve(convex_polygon.size());
    for(const Point& p: convex_polygon){
        if(vertices.empty() || !(vertices.back() == p)) vertices.push_back(p);
    }
    if(vertices.size() >= 2 && vertices.front() == vertices.back()) vertices.pop_back();
    int count = static_cast<int>(vertices.size());
    if(count <= 1) return 0.0L;
    if(count == 2) return distance(vertices[0], vertices[1]);

    int start = -1;
    for(int i = 0; i < count; i++){
        const Point& previous = vertices[static_cast<std::size_t>((i + count - 1) % count)];
        const Point& current = vertices[static_cast<std::size_t>(i)];
        const Point& next = vertices[static_cast<std::size_t>((i + 1) % count)];
        if(geometry_sign(cross(current - previous, next - current)) != 0){
            start = i;
            break;
        }
    }
    if(start == -1){
        auto [minimum, maximum] = std::minmax_element(vertices.begin(), vertices.end());
        return distance(*minimum, *maximum);
    }

    std::vector<Point> polygon;
    polygon.reserve(vertices.size());
    for(int offset = 0; offset < count; offset++){
        const Point& p = vertices[static_cast<std::size_t>((start + offset) % count)];
        while(polygon.size() >= 2 && geometry_sign(cross(
            polygon.back() - polygon[polygon.size() - 2],
            p - polygon.back()
        )) == 0){
            polygon.pop_back();
        }
        polygon.push_back(p);
    }

    int n = static_cast<int>(polygon.size());
    if(n == 2) return distance(polygon[0], polygon[1]);
    long double result = 0.0L;
    int j = 1;
    for(int i = 0; i < n; i++){
        int next_i = (i + 1) % n;
        Point edge = polygon[static_cast<std::size_t>(next_i)] - polygon[static_cast<std::size_t>(i)];
        while(true){
            int next_j = (j + 1) % n;
            long double current_area = std::abs(cross(
                edge,
                polygon[static_cast<std::size_t>(j)] - polygon[static_cast<std::size_t>(i)]
            ));
            long double next_area = std::abs(cross(
                edge,
                polygon[static_cast<std::size_t>(next_j)] - polygon[static_cast<std::size_t>(i)]
            ));
            if(geometry_sign(next_area - current_area) <= 0) break;
            j = next_j;
        }
        result = std::max(result, distance(
            polygon[static_cast<std::size_t>(i)],
            polygon[static_cast<std::size_t>(j)]
        ));
        result = std::max(result, distance(
            polygon[static_cast<std::size_t>(next_i)],
            polygon[static_cast<std::size_t>(j)]
        ));
    }
    return result;
}
inline std::vector<Point> convex_cut(const std::vector<Point>& polygon, const Line& line){
    if(geometry_sign(abs(line.b - line.a)) == 0) throw std::invalid_argument("degenerate line");
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

enum CircleRelation{
    CIRCLE_INSIDE = 0,
    CIRCLE_INTERNALLY_TANGENT = 1,
    CIRCLE_INTERSECT = 2,
    CIRCLE_EXTERNALLY_TANGENT = 3,
    CIRCLE_SEPARATE = 4,
    CIRCLE_COINCIDENT = 5,
};

inline void validate_circle(const Circle& circle){
    if(circle.radius < 0.0L) throw std::invalid_argument("negative circle radius");
}

inline int circle_relation(const Circle& a, const Circle& b){
    validate_circle(a);
    validate_circle(b);
    long double d = distance(a.center, b.center);
    long double rsum = a.radius + b.radius;
    long double rdiff = std::abs(a.radius - b.radius);
    if(geometry_sign(d) == 0 && geometry_sign(rdiff) == 0) return CIRCLE_COINCIDENT;
    if(geometry_sign(d - rsum) > 0) return CIRCLE_SEPARATE;
    if(geometry_sign(d - rsum) == 0) return CIRCLE_EXTERNALLY_TANGENT;
    if(geometry_sign(d - rdiff) == 0) return CIRCLE_INTERNALLY_TANGENT;
    if(geometry_sign(d - rdiff) < 0) return CIRCLE_INSIDE;
    return CIRCLE_INTERSECT;
}

inline bool intersect_circle_line(const Circle& circle, const Line& line){
    validate_circle(circle);
    return geometry_sign(distance(line, circle.center) - circle.radius) <= 0;
}

inline bool intersect_circle_segment(const Circle& circle, const Segment& segment){
    validate_circle(circle);
    long double min_distance = distance_segment_point(segment, circle.center);
    long double max_distance = std::max(distance(circle.center, segment.a), distance(circle.center, segment.b));
    return geometry_sign(min_distance - circle.radius) <= 0 && geometry_sign(circle.radius - max_distance) <= 0;
}

inline bool intersect_circle_circle(const Circle& a, const Circle& b){
    int relation = circle_relation(a, b);
    return relation != CIRCLE_INSIDE && relation != CIRCLE_SEPARATE;
}

inline std::vector<Point> circle_line_cross_points(const Circle& circle, const Line& line){
    validate_circle(circle);
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
    validate_circle(circle);
    if(geometry_sign(abs(segment.b - segment.a)) == 0){
        return geometry_sign(distance(circle.center, segment.a) - circle.radius) == 0
                   ? std::vector<Point>{segment.a}
                   : std::vector<Point>{};
    }
    std::vector<Point> result;
    for(const Point& p: circle_line_cross_points(circle, segment)){
        if(ccw(segment.a, segment.b, p) == ON_SEGMENT) result.push_back(p);
    }
    return result;
}

inline std::vector<Point> circle_circle_cross_points(const Circle& a, const Circle& b){
    int relation = circle_relation(a, b);
    if(relation == CIRCLE_COINCIDENT) throw std::domain_error("coincident circles have infinitely many intersections");
    if(relation == CIRCLE_INSIDE || relation == CIRCLE_SEPARATE) return {};
    long double d = distance(a.center, b.center);
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
    validate_circle(circle);
    if(geometry_sign(circle.radius) == 0) return {circle.center};
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
    validate_circle(circle);
    if(geometry_sign(circle.radius) == 0){
        if(p == circle.center) throw std::domain_error("a point circle has no unique tangent at its center");
        return {{p, circle.center}};
    }
    std::vector<Line> result;
    for(const Point& q: tangent_points(circle, p)){
        if(q == p) result.push_back({q, q + rotate90(q - circle.center)});
        else result.push_back({p, q});
    }
    return result;
}

inline std::vector<Line> common_tangents(const Circle& a, const Circle& b){
    validate_circle(a);
    validate_circle(b);
    Point v = b.center - a.center;
    long double d2 = norm(v);
    if(geometry_sign(abs(v)) == 0 && geometry_sign(a.radius - b.radius) == 0){
        throw std::domain_error("coincident circles have infinitely many common tangents");
    }
    if(geometry_sign(abs(v)) == 0) return {};
    std::vector<Line> result;
    for(int same_side: {-1, 1}){
        long double r = a.radius - same_side * b.radius;
        long double h2 = d2 - r * r;
        if(geometry_sign(h2) < 0) continue;
        long double h = std::sqrt(std::max<long double>(0, h2));
        for(int sign: {-1, 1}){
            Point normal = (v * r + rotate90(v) * (h * sign)) / d2;
            Point p1 = a.center + normal * a.radius;
            Point p2 = b.center + normal * (same_side * b.radius);
            Line tangent = p1 == p2 ? Line{p1, p1 + rotate90(normal)} : Line{p1, p2};
            bool duplicate = std::any_of(result.begin(), result.end(), [&](const Line& line){
                return same_line(line, tangent);
            });
            if(!duplicate) result.push_back(tangent);
            if(geometry_sign(h) == 0) break;
        }
    }
    return result;
}
inline Point circumcenter(const Point& a, const Point& b, const Point& c){
    if(geometry_sign(cross(b - a, c - a)) == 0) throw std::invalid_argument("collinear triangle");
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
    if(geometry_sign(cross(b - a, c - a)) == 0) throw std::invalid_argument("collinear triangle");
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
        if(a.x != b.x) return a.x < b.x;
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
                if(a.y != b.y) return a.y < b.y;
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