#pragma once

#include <algorithm>
#include <cmath>
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

    Point operator+() const{
        return *this;
    }

    Point operator-() const{
        return {-x, -y};
    }

    Point operator+(const Point& other) const{
        return {x + other.x, y + other.y};
    }

    Point operator-(const Point& other) const{
        return {x - other.x, y - other.y};
    }

    Point operator*(long double k) const{
        return {x * k, y * k};
    }

    Point operator/(long double k) const{
        return {x / k, y / k};
    }

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

inline Point rotate90(const Point& p){
    return {-p.y, p.x};
}

inline Point rotate(const Point& p, long double theta){
    long double c = std::cos(theta);
    long double s = std::sin(theta);
    return {p.x * c - p.y * s, p.x * s + p.y * c};
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

inline Point cross_point(const Line& a, const Line& b){
    long double d1 = std::abs(cross(b.b - b.a, a.a - b.a));
    long double d2 = std::abs(cross(b.b - b.a, a.b - b.a));
    return a.a + (a.b - a.a) * (d1 / (d1 + d2));
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
