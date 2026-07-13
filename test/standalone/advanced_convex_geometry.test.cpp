// competitive-verifier: STANDALONE

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/geometry/2d/advanced_convex_geometry.hpp"

namespace{

constexpr long double OUTPUT_SCALE = 1000000.0L;

std::vector<Point> read_polygon(){
    int count;
    std::cin >> count;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    return polygon;
}

void print_polygon(const std::vector<Point>& polygon, char prefix){
    std::cout << prefix << ' ' << polygon.size();
    for(const Point& point: polygon){
        std::cout << ' ' << std::llround(point.x * OUTPUT_SCALE)
                  << ' ' << std::llround(point.y * OUTPUT_SCALE);
    }
    std::cout << '\n';
}

void self_check(){
    if(half_plane_intersection({}).kind != HALF_PLANE_UNBOUNDED){
        throw std::runtime_error("the whole plane must be unbounded");
    }
    bool invalid_line_thrown = false;
    try{
        (void)half_plane_intersection({{{0, 0}, {0, 0}}});
    }catch(const std::invalid_argument&){
        invalid_line_thrown = true;
    }
    if(!invalid_line_thrown) throw std::runtime_error("a degenerate boundary was accepted");

    bool nonconvex_thrown = false;
    try{
        (void)normalize_convex_polygon({{0, 0}, {2, 0}, {1, 1}, {2, 2}, {0, 2}});
    }catch(const std::invalid_argument&){
        nonconvex_thrown = true;
    }
    if(!nonconvex_thrown) throw std::runtime_error("a non-convex polygon was accepted");
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_check();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    for(int query = 0; query < query_count; ++query){
        char operation;
        std::cin >> operation;
        if(operation == 'H'){
            int count;
            std::cin >> count;
            std::vector<Line> half_planes(static_cast<std::size_t>(count));
            for(Line& half_plane: half_planes){
                std::cin >> half_plane.a.x >> half_plane.a.y
                         >> half_plane.b.x >> half_plane.b.y;
            }
            HalfPlaneIntersectionResult result = half_plane_intersection(
                std::move(half_planes)
            );
            if(result.kind == HALF_PLANE_EMPTY) std::cout << "E\n";
            else if(result.kind == HALF_PLANE_UNBOUNDED) std::cout << "U\n";
            else print_polygon(result.polygon, 'B');
        }else if(operation == 'M'){
            std::vector<Point> first = read_polygon();
            std::vector<Point> second = read_polygon();
            print_polygon(
                convex_polygon_minkowski_sum(std::move(first), std::move(second)),
                'P'
            );
        }else if(operation == 'I'){
            std::vector<Point> first = read_polygon();
            std::vector<Point> second = read_polygon();
            print_polygon(
                convex_polygon_intersection(std::move(first), std::move(second)),
                'P'
            );
        }else{
            throw std::runtime_error("unknown operation");
        }
    }
}
