#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<
    class UInt,
    int BITS = std::numeric_limits<UInt>::digits
>
struct XorBasis{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(0 < BITS);
    static_assert(BITS <= std::numeric_limits<UInt>::digits);

private:
    std::array<UInt, static_cast<std::size_t>(BITS)> basis{};
    int _rank = 0;

    static void check_value(UInt value, const char* message){
        if constexpr(BITS < std::numeric_limits<UInt>::digits){
            if((value >> BITS) != 0)[[unlikely]]{
                throw std::runtime_error(message);
            }
        }
    }

public:
    XorBasis() = default;

    void clear(){
        basis.fill(UInt(0));
        _rank = 0;
    }

    int rank() const{
        return _rank;
    }

    bool empty() const{
        return _rank == 0;
    }

    bool insert(UInt value){
        check_value(value, "library assertion fault: range violation (XorBasis::insert).");
        for(int bit = BITS - 1; bit >= 0; bit--){
            if(((value >> bit) & UInt(1)) != 0 && basis[static_cast<std::size_t>(bit)] != 0){
                value ^= basis[static_cast<std::size_t>(bit)];
            }
        }
        if(value == 0) return false;

        int pivot = BITS - 1;
        while(((value >> pivot) & UInt(1)) == 0) pivot--;
        for(int bit = 0; bit < BITS; bit++){
            if(bit != pivot && ((basis[static_cast<std::size_t>(bit)] >> pivot) & UInt(1)) != 0){
                basis[static_cast<std::size_t>(bit)] ^= value;
            }
        }
        basis[static_cast<std::size_t>(pivot)] = value;
        _rank++;
        return true;
    }

    bool contains(UInt value) const{
        check_value(value, "library assertion fault: range violation (XorBasis::contains).");
        for(int bit = BITS - 1; bit >= 0; bit--){
            if(((value >> bit) & UInt(1)) != 0 && basis[static_cast<std::size_t>(bit)] != 0){
                value ^= basis[static_cast<std::size_t>(bit)];
            }
        }
        return value == 0;
    }

    UInt maximize(UInt value = 0) const{
        check_value(value, "library assertion fault: range violation (XorBasis::maximize).");
        for(int bit = BITS - 1; bit >= 0; bit--){
            const UInt candidate = value ^ basis[static_cast<std::size_t>(bit)];
            if(value < candidate) value = candidate;
        }
        return value;
    }

    UInt minimize(UInt value) const{
        check_value(value, "library assertion fault: range violation (XorBasis::minimize).");
        for(int bit = BITS - 1; bit >= 0; bit--){
            const UInt candidate = value ^ basis[static_cast<std::size_t>(bit)];
            if(candidate < value) value = candidate;
        }
        return value;
    }

    UInt kth_smallest(unsigned long long k) const{
        if(_rank < std::numeric_limits<unsigned long long>::digits &&
            k >= (1ULL << _rank))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (XorBasis::kth_smallest).");
        }
        UInt result = 0;
        int coefficient = 0;
        for(int bit = 0; bit < BITS; bit++){
            if(basis[static_cast<std::size_t>(bit)] == 0) continue;
            if(((k >> coefficient) & 1ULL) != 0){
                result ^= basis[static_cast<std::size_t>(bit)];
            }
            coefficient++;
        }
        return result;
    }

    std::vector<UInt> vectors() const{
        std::vector<UInt> result;
        result.reserve(static_cast<std::size_t>(_rank));
        for(int bit = 0; bit < BITS; bit++){
            if(basis[static_cast<std::size_t>(bit)] != 0){
                result.push_back(basis[static_cast<std::size_t>(bit)]);
            }
        }
        return result;
    }

    void merge(const XorBasis& other){
        for(UInt value: other.basis){
            if(value != 0) insert(value);
        }
    }
};
