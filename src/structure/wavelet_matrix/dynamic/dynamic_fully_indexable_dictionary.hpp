#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_DYNAMIC_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_DYNAMIC_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../detail/mutable_btree_bit_sequence.hpp"

template<int MAX_SIZE>
struct DynamicFullyIndexableDictionary{
    static_assert(MAX_SIZE >= 0);

private:
    using Storage = wavelet_matrix_detail::MutableBTreeBitSequence<
        wavelet_matrix_detail::MutableBTreeNoPayload,
        wavelet_matrix_detail::MutableBTreeNoSum,
        wavelet_matrix_detail::MutableBTreeNoPayloadLift
    >;

    Storage bits;

    static std::vector<unsigned char> unpack(
        const std::vector<bool>& values
    ){
        std::vector<unsigned char> result(values.size());
        for(std::size_t index = 0; index < values.size(); index++){
            result[index] = values[index] ? 1 : 0;
        }
        return result;
    }

    void check_index(int k, const char* message) const{
        if(k < 0 || size() <= k)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    explicit DynamicFullyIndexableDictionary(int n = 0){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        bits.assign(std::vector<unsigned char>(
            static_cast<std::size_t>(n), 0
        ));
    }

    explicit DynamicFullyIndexableDictionary(
        const std::vector<bool>& values
    ){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        bits.assign(unpack(values));
    }

    DynamicFullyIndexableDictionary(
        const DynamicFullyIndexableDictionary&
    ) = default;
    DynamicFullyIndexableDictionary& operator=(
        const DynamicFullyIndexableDictionary&
    ) = default;
    DynamicFullyIndexableDictionary(
        DynamicFullyIndexableDictionary&&
    ) noexcept = default;
    DynamicFullyIndexableDictionary& operator=(
        DynamicFullyIndexableDictionary&&
    ) noexcept = default;

    int size() const{ return bits.size(); }

    bool operator[](int k) const{
        check_index(
            k,
            "library assertion fault: range violation (operator[])."
        );
        return bits.access(k).bit;
    }

    void set(int k, bool value){
        check_index(k, "library assertion fault: range violation (set).");
        bits.set_bit(k, value);
    }

    void flip(int k){
        check_index(k, "library assertion fault: range violation (flip).");
        bits.flip(k);
    }

    void insert(int position, bool value){
        if(position < 0 || size() < position || size() == MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (insert)."
            );
        }
        bits.insert(position, value);
    }

    bool erase(int position){
        check_index(
            position,
            "library assertion fault: range violation (erase)."
        );
        return bits.erase(position).bit;
    }

    void push_back(bool value){ insert(size(), value); }

    bool pop_back(){
        if(size() == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (pop_back)."
            );
        }
        return erase(size() - 1);
    }

    int rank(bool value, int r) const{
        if(r < 0 || size() < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rank)."
            );
        }
        return bits.rank(value, r);
    }

    int rank(bool value, int l, int r) const{
        if(l < 0 || r < l || size() < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rank)."
            );
        }
        return bits.rank(value, r) - bits.rank(value, l);
    }

    int select(bool value, int k) const{
        if(k < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (select)."
            );
        }
        return bits.select(value, k);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_DYNAMIC_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED
