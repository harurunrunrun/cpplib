#ifndef CPPLIB_SRC_STRUCTURE_ARRAY_STATIC_OR_VECTOR_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ARRAY_STATIC_OR_VECTOR_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace cpplib_detail{

inline constexpr int checked_int_size(std::size_t n){
    if(n > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error("array size exceeds the index type");
    }
    return static_cast<int>(n);
}

template<class T, std::ptrdiff_t Capacity>
class StaticOrVector{
    static_assert(Capacity >= -1);
    using Container = std::conditional_t<Capacity == -1, std::vector<T>,
        std::array<T, static_cast<std::size_t>(Capacity < 0 ? 0 : Capacity)>>;
    Container values{};

    static constexpr void check_size(std::size_t n){
        if constexpr(Capacity >= 0){
            if(n > static_cast<std::size_t>(Capacity)){
                throw std::length_error("fixed storage capacity exceeded");
            }
        }
    }

public:
    using value_type = T;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    static constexpr bool is_dynamic = Capacity == -1;

    constexpr StaticOrVector() = default;
    explicit constexpr StaticOrVector(std::size_t n){resize(n);}
    constexpr void resize(std::size_t n){
        check_size(n);
        if constexpr(is_dynamic) values.resize(n);
    }
    constexpr void resize(std::size_t n, const T& value){
        check_size(n);
        if constexpr(is_dynamic) values.resize(n, value);
    }
    constexpr void reserve(std::size_t n){
        check_size(n);
        if constexpr(is_dynamic) values.reserve(n);
    }
    constexpr void ensure(std::size_t n){
        if(n > size()) resize(n);
    }
    constexpr void fill(const T& value){std::fill(begin(), end(), value);}
    constexpr std::size_t size() const noexcept{return values.size();}
    constexpr bool empty() const noexcept{return values.empty();}
    constexpr std::size_t capacity() const noexcept{
        if constexpr(is_dynamic) return values.capacity();
        else return static_cast<std::size_t>(Capacity);
    }
    constexpr reference operator[](std::size_t i) noexcept{return values[i];}
    constexpr const_reference operator[](std::size_t i) const noexcept{return values[i];}
    constexpr reference at(std::size_t i){return values.at(i);}
    constexpr const_reference at(std::size_t i) const{return values.at(i);}
    constexpr iterator begin() noexcept{return values.begin();}
    constexpr const_iterator begin() const noexcept{return values.begin();}
    constexpr iterator end() noexcept{return values.end();}
    constexpr const_iterator end() const noexcept{return values.end();}
    constexpr const_iterator cbegin() const noexcept{return values.cbegin();}
    constexpr const_iterator cend() const noexcept{return values.cend();}
    constexpr auto data() noexcept{return values.data();}
    constexpr auto data() const noexcept{return values.data();}
    constexpr reference front() noexcept{return values.front();}
    constexpr const_reference front() const noexcept{return values.front();}
    constexpr reference back() noexcept{return values.back();}
    constexpr const_reference back() const noexcept{return values.back();}
    constexpr void swap(StaticOrVector& other)
        noexcept(noexcept(values.swap(other.values))){values.swap(other.values);}
};

} // namespace cpplib_detail

#endif  // CPPLIB_SRC_STRUCTURE_ARRAY_STATIC_OR_VECTOR_HPP_INCLUDED
