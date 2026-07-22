#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_COMPRESSED_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_COMPRESSED_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>
#include "weighted_wavelet_matrix.hpp"

template<
    class T,
    class W,
    int MAX_SIZE,
    int ID_BIT_WIDTH = std::numeric_limits<unsigned>::digits
>
struct CompressedWeightedWaveletMatrix{
    static_assert(MAX_SIZE >= 0);
    static_assert(0 < ID_BIT_WIDTH && ID_BIT_WIDTH <= std::numeric_limits<unsigned>::digits);

private:
    int _n = 0;
    std::vector<T> values;
    std::unique_ptr<WeightedWaveletMatrix<int, W, MAX_SIZE, ID_BIT_WIDTH>> matrix;

    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]] throw std::runtime_error(message);
    }
    int lower_id(const T& value) const{
        return static_cast<int>(std::lower_bound(values.begin(), values.end(), value) -
            values.begin());
    }
    std::optional<int> find_id(const T& value) const{
        int id = lower_id(value);
        if(id == static_cast<int>(values.size()) || values[static_cast<std::size_t>(id)] != value){
            return std::nullopt;
        }
        return id;
    }
    int count_id_less(int l, int r, int upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        if(upper <= 0) return 0;
        if(static_cast<int>(values.size()) <= upper) return r - l;
        return matrix->range_freq(l, r, upper);
    }
    W sum_id_less(int l, int r, int upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        if(upper <= 0) return W{};
        if(static_cast<int>(values.size()) <= upper) return matrix->sum(l, r);
        return matrix->range_sum(l, r, upper);
    }

public:
    CompressedWeightedWaveletMatrix(
        const std::vector<T>& sequence,
        const std::vector<W>& weights
    ): _n(static_cast<int>(sequence.size())), values(sequence), matrix(nullptr){
        if(sequence.size() != weights.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        if(sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
        std::vector<int> ids(sequence.size());
        for(int k = 0; k < _n; k++){
            ids[static_cast<std::size_t>(k)] = lower_id(sequence[static_cast<std::size_t>(k)]);
        }
        matrix = std::make_unique<WeightedWaveletMatrix<int, W, MAX_SIZE, ID_BIT_WIDTH>>(
            ids, weights);
    }

    template<std::size_t N>
    CompressedWeightedWaveletMatrix(
        const std::array<T, N>& sequence,
        const std::array<W, N>& weights
    ): CompressedWeightedWaveletMatrix(
        std::vector<T>(sequence.begin(), sequence.end()),
        std::vector<W>(weights.begin(), weights.end())
    ){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    CompressedWeightedWaveletMatrix(const CompressedWeightedWaveletMatrix&) = delete;
    CompressedWeightedWaveletMatrix& operator=(const CompressedWeightedWaveletMatrix&) = delete;
    CompressedWeightedWaveletMatrix(CompressedWeightedWaveletMatrix&&) = default;
    CompressedWeightedWaveletMatrix& operator=(CompressedWeightedWaveletMatrix&&) = default;

    int size() const{ return _n; }
    int value_count() const{ return static_cast<int>(values.size()); }
    const T& value(int id) const{
        if(id < 0 || static_cast<int>(values.size()) <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (value).");
        }
        return values[static_cast<std::size_t>(id)];
    }

    T access(int k) const{ return values[static_cast<std::size_t>(matrix->access(k))]; }
    T operator[](int k) const{ return access(k); }

    int rank(const T& value, int r) const{ return rank(value, 0, r); }
    int rank(const T& value, int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (rank).");
        auto id = find_id(value);
        if(!id) return 0;
        return matrix->rank(*id, l, r);
    }

    T kth_smallest(int l, int r, int k) const{
        return values[static_cast<std::size_t>(matrix->kth_smallest(l, r, k))];
    }
    T kth_largest(int l, int r, int k) const{
        return values[static_cast<std::size_t>(matrix->kth_largest(l, r, k))];
    }
    T quantile(int l, int r, int k) const{ return kth_smallest(l, r, k); }

    int range_freq(int l, int r, const T& upper) const{
        return count_id_less(l, r, lower_id(upper));
    }
    int range_freq(int l, int r, const T& lower, const T& upper) const{
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_freq).");
        }
        return count_id_less(l, r, lower_id(upper)) -
            count_id_less(l, r, lower_id(lower));
    }

    W sum(int l, int r) const{ return matrix->sum(l, r); }
    W range_sum(int l, int r, const T& upper) const{
        return sum_id_less(l, r, lower_id(upper));
    }
    W range_sum(int l, int r, const T& lower, const T& upper) const{
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_sum).");
        }
        return sum_id_less(l, r, lower_id(upper)) -
            sum_id_less(l, r, lower_id(lower));
    }
    W sum_k_smallest(int l, int r, int k) const{
        return matrix->sum_k_smallest(l, r, k);
    }
    W sum_k_largest(int l, int r, int k) const{
        return matrix->sum_k_largest(l, r, k);
    }

    T range_min(int l, int r) const{ return kth_smallest(l, r, 0); }
    T range_max(int l, int r) const{ return kth_largest(l, r, 0); }
    T median_floor(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (median_floor).");
        if(l == r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (median_floor).");
        }
        return kth_smallest(l, r, (r - l - 1) / 2);
    }
    T median_ceil(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (median_ceil).");
        if(l == r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (median_ceil).");
        }
        return kth_smallest(l, r, (r - l) / 2);
    }

    std::optional<T> prev_value(int l, int r, const T& upper) const{
        int count = range_freq(l, r, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(l, r, count - 1);
    }
    std::optional<T> next_value(int l, int r, const T& lower) const{
        int count = range_freq(l, r, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest(l, r, count);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_COMPRESSED_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
