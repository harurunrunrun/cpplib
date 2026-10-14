#ifndef CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_TREE_2D_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_TREE_2D_HPP_INCLUDED

#include <array>
#include <type_traits>
#include "../array/static_or_vector.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>

template<class T, int MAX_H = -1, int MAX_W = -1>
struct FenwickTree2D{
    static_assert(MAX_H == -1 || MAX_H > 0);
    static_assert(MAX_W == -1 || MAX_W > 0);

    int _h, _w;
    cpplib_detail::StaticOrVector<T, (MAX_H < 0 || MAX_W < 0 ? -1 : (static_cast<std::ptrdiff_t>(MAX_H) + 1) * (static_cast<std::ptrdiff_t>(MAX_W) + 1))> data;

    explicit FenwickTree2D(int h = (MAX_H < 0 ? 0 : MAX_H), int w = (MAX_W < 0 ? 0 : MAX_W)) : _h(h), _w(w){
        if(_h < 0 || (MAX_H >= 0 && MAX_H < _h) || _w < 0 || (MAX_W >= 0 && MAX_W < _w)) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.resize((static_cast<std::size_t>(_h) + 1) * (MAX_W < 0 ? static_cast<std::size_t>(_w) + 1 : static_cast<std::size_t>(MAX_W) + 1));
        data.fill(T{});
    }

    explicit FenwickTree2D(const std::vector<std::vector<T>>& v)
        : FenwickTree2D(cpplib_detail::checked_int_size(v.size()), v.empty() ? 0 : cpplib_detail::checked_int_size(v[0].size())){
        for(int y = 0; y < _h; y++){
            if(v[y].size() != static_cast<std::size_t>(_w)) [[unlikely]] {
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
        for(std::size_t i = static_cast<std::size_t>(y) + 1; i <= static_cast<std::size_t>(_h); i += i & -i){
            for(std::size_t j = static_cast<std::size_t>(x) + 1; j <= static_cast<std::size_t>(_w); j += j & -j){
                if constexpr(std::is_same_v<T, bool>) data[index(i, j)] = static_cast<T>(data[index(i, j)]) + value;
                else data[index(i, j)] += value;
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
        constexpr std::size_t index(std::size_t y, std::size_t x) const{
            return static_cast<std::size_t>(y) * (MAX_W < 0 ? static_cast<std::size_t>(_w) + 1 : static_cast<std::size_t>(MAX_W) + 1) + x;
        }
};

#endif  // CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_TREE_2D_HPP_INCLUDED
