#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

template<class T, int MAX_H, int MAX_W>
struct FenwickTree2D{
    static_assert(MAX_H > 0);
    static_assert(MAX_W > 0);

    int _h, _w;
    std::array<T, (MAX_H + 1) * (MAX_W + 1)> data;

    explicit FenwickTree2D(int h = MAX_H, int w = MAX_W) : _h(h), _w(w){
        if(_h < 0 || MAX_H < _h || _w < 0 || MAX_W < _w) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(T{});
    }

    explicit FenwickTree2D(const std::vector<std::vector<T>>& v)
        : FenwickTree2D(static_cast<int>(v.size()), v.empty() ? 0 : static_cast<int>(v[0].size())){
        for(int y = 0; y < _h; y++){
            if(static_cast<int>(v[y].size()) != _w) [[unlikely]] {
                throw std::runtime_error("library assertion fault: invalid shape (constructor).");
            }
            for(int x = 0; x < _w; x++){
                add(y, x, v[y][x]);
            }
        }
    }

    int height()const{
        return _h;
    }

    int width()const{
        return _w;
    }

    bool empty()const{
        return _h == 0 || _w == 0;
    }

    void add(int y, int x, const T& value){
        if(y < 0 || _h <= y || x < 0 || _w <= x) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (add).");
        }
        for(int i = y + 1; i <= _h; i += i & -i){
            for(int j = x + 1; j <= _w; j += j & -j){
                data[index(i, j)] += value;
            }
        }
    }

    T prefix_sum(int y, int x)const{
        if(y < 0 || _h < y || x < 0 || _w < x) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (prefix_sum).");
        }
        T res{};
        for(int i = y; i > 0; i -= i & -i){
            for(int j = x; j > 0; j -= j & -j){
                res += data[index(i, j)];
            }
        }
        return res;
    }

    T sum(int y1, int x1, int y2, int x2)const{
        if(y1 < 0 || y1 > y2 || _h < y2 || x1 < 0 || x1 > x2 || _w < x2) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (sum).");
        }
        return prefix_sum(y2, x2) - prefix_sum(y1, x2) - prefix_sum(y2, x1) + prefix_sum(y1, x1);
    }

    T get(int y, int x)const{
        if(y < 0 || _h <= y || x < 0 || _w <= x) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return sum(y, x, y + 1, x + 1);
    }

    void set(int y, int x, const T& value){
        if(y < 0 || _h <= y || x < 0 || _w <= x) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (set).");
        }
        add(y, x, value - get(y, x));
    }

    private:
        static constexpr int index(int y, int x){
            return y * (MAX_W + 1) + x;
        }
};
