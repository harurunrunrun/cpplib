#ifndef CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_CONVEX_HULL_TRICK_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_CONVEX_HULL_TRICK_HPP_INCLUDED

#include <array>
#include <limits>
#include <stdexcept>

template<int MAX_LINE>
struct ConvexHullTrick{
    static_assert(MAX_LINE >= 0);

    static constexpr long long INF =
        std::numeric_limits<long long>::max() / 4;

    struct Line{
        long long a;
        long long b;

        __int128 eval128(long long x) const{
            return static_cast<__int128>(a) * x + b;
        }

        long long eval(long long x) const{
            __int128 y = eval128(x);
            if(y > std::numeric_limits<long long>::max()){
                return std::numeric_limits<long long>::max();
            }
            if(y < std::numeric_limits<long long>::min()){
                return std::numeric_limits<long long>::min();
            }
            return static_cast<long long>(y);
        }
    };

private:
    int line_count = 0;
    int pointer = 0;
    std::array<Line, MAX_LINE> hull;

    static bool product_ge(
        __int128 lhs_x, __int128 lhs_y,
        __int128 rhs_x, __int128 rhs_y
    ){
        bool lhs_negative = (lhs_x < 0) != (lhs_y < 0);
        bool rhs_negative = (rhs_x < 0) != (rhs_y < 0);
        unsigned __int128 lhs = static_cast<unsigned __int128>(
            lhs_x < 0 ? -lhs_x : lhs_x) * static_cast<unsigned __int128>(
            lhs_y < 0 ? -lhs_y : lhs_y);
        unsigned __int128 rhs = static_cast<unsigned __int128>(
            rhs_x < 0 ? -rhs_x : rhs_x) * static_cast<unsigned __int128>(
            rhs_y < 0 ? -rhs_y : rhs_y);
        if(lhs == 0) lhs_negative = false;
        if(rhs == 0) rhs_negative = false;
        if(lhs_negative != rhs_negative) return !lhs_negative;
        return lhs_negative ? lhs <= rhs : lhs >= rhs;
    }

    static bool unnecessary(const Line& f, const Line& g, const Line& h){
        return product_ge(
            static_cast<__int128>(g.b) - f.b,
            static_cast<__int128>(g.a) - h.a,
            static_cast<__int128>(h.b) - g.b,
            static_cast<__int128>(f.a) - g.a
        );
    }

    void check_not_empty(const char* message) const{
        if(line_count == 0)[[unlikely]] throw std::runtime_error(message);
    }

public:
    int size() const{ return line_count; }
    bool empty() const{ return line_count == 0; }

    void add_line(long long a, long long b){
        Line line{a, b};

        if(line_count > 0 && a > hull[line_count - 1].a)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: monotonicity violation (add_line)."
            );
        }

        if(line_count > 0 && a == hull[line_count - 1].a){
            if(hull[line_count - 1].b <= b) return;
            line_count--;
            if(pointer > line_count) pointer = line_count;
        }


        while(
            line_count >= 2 &&
            unnecessary(hull[line_count - 2], hull[line_count - 1], line)
        ){
            line_count--;
            if(pointer > line_count) pointer = line_count;
        }

        if(line_count >= MAX_LINE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: line capacity exceeded (add_line)."
            );
        }

        hull[line_count++] = line;
    }

    long long query(long long x) const{
        check_not_empty("library assertion fault: empty hull (query).");

        int low = 0;
        int high = line_count - 1;
        while(low < high){
            int mid = (low + high) / 2;
            if(hull[mid].eval128(x) <= hull[mid + 1].eval128(x)){
                high = mid;
            }else{
                low = mid + 1;
            }
        }
        return hull[low].eval(x);
    }

    long long query_monotone_inc(long long x){
        check_not_empty(
            "library assertion fault: empty hull (query_monotone_inc)."
        );

        while(
            pointer + 1 < line_count &&
            hull[pointer + 1].eval128(x) <= hull[pointer].eval128(x)
        ){
            pointer++;
        }
        return hull[pointer].eval(x);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_CONVEX_HULL_TRICK_HPP_INCLUDED
