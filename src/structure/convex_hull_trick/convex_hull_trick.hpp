#pragma once

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
            if(y > INF) return INF;
            if(y < -static_cast<__int128>(INF)) return -INF;
            return static_cast<long long>(y);
        }
    };

private:
    int line_count = 0;
    int pointer = 0;
    std::array<Line, MAX_LINE> hull;

    static bool unnecessary(const Line& f, const Line& g, const Line& h){
        return
            static_cast<__int128>(g.b - f.b) * (g.a - h.a) >=
            static_cast<__int128>(h.b - g.b) * (f.a - g.a);
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

        if(line_count >= MAX_LINE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: line capacity exceeded (add_line)."
            );
        }

        while(
            line_count >= 2 &&
            unnecessary(hull[line_count - 2], hull[line_count - 1], line)
        ){
            line_count--;
            if(pointer > line_count) pointer = line_count;
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
