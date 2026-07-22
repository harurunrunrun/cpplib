#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PERSISTENT_WEIGHTED_WAVELET_MATRIX_BASE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PERSISTENT_WEIGHTED_WAVELET_MATRIX_BASE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "persistent_block_reference.hpp"

namespace wavelet_matrix_detail{

template<class T, class W, int MAX_SIZE, int MAX_VERSION, int BIT_WIDTH, int BLOCK_SIZE>
struct PersistentWeightedWaveletMatrixBase{
    static_assert(std::is_integral_v<T>);
    static_assert(MAX_SIZE >= 0);
    static_assert(MAX_VERSION >= 0);
    static_assert(BLOCK_SIZE > 0);
    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;
    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);

private:
    static constexpr int block_count_max = (MAX_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
    static constexpr int block_node_max = block_count_max + MAX_VERSION + 2;

    int _n = 0;
    int block_count = 0;
    int _versions = 0;
    int block_nodes = 0;
    std::array<int, MAX_VERSION + 1> version_root{};
    PersistentBlockReference<block_count_max, MAX_VERSION> block_reference;
    std::array<std::array<U, BLOCK_SIZE>, block_node_max> block_value{};
    std::array<std::array<W, BLOCK_SIZE>, block_node_max> block_weight{};
    std::array<std::array<U, BLOCK_SIZE>, block_node_max> sorted_value{};
    std::array<std::array<W, BLOCK_SIZE + 1>, block_node_max> sorted_prefix{};
    std::array<W, block_node_max> block_sum{};
    std::array<int, block_node_max> block_size{};

    static constexpr U sign_mask = std::is_signed_v<T>
        ? U{1} << (digits - 1) : U{0};
    static U encode(T value){ return static_cast<U>(value) ^ sign_mask; }
    static T decode(U value){ return static_cast<T>(value ^ sign_mask); }
    static bool fits(U value){
        if constexpr(BIT_WIDTH < digits) return (value >> BIT_WIDTH) == 0;
        else return true;
    }
    static U encode_checked(T value, const char* message){
        U result = encode(value);
        if(!fits(result))[[unlikely]] throw std::runtime_error(message);
        return result;
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || _versions <= version)[[unlikely]] throw std::runtime_error(message);
    }
    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]] throw std::runtime_error(message);
    }
    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]] throw std::runtime_error(message);
    }
    void check_version_capacity() const{
        if(_versions > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity violation (version).");
        }
    }
    int new_block(){
        if(block_nodes >= block_node_max)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity violation (block).");
        }
        return block_nodes++;
    }
    int block_index(int k) const{ return k / BLOCK_SIZE; }
    int local_index(int k) const{ return k % BLOCK_SIZE; }
    int actual_block_size(int block) const{
        int first = block * BLOCK_SIZE;
        return std::min(_n - first, BLOCK_SIZE);
    }

    void rebuild_block(int node){
        std::array<std::pair<U, W>, BLOCK_SIZE> buffer{};
        W total{};
        std::size_t size = static_cast<std::size_t>(
            std::clamp(block_size[static_cast<std::size_t>(node)], 0, BLOCK_SIZE));
        for(std::size_t i = 0; i < size; i++){
            buffer[i] = {
                block_value[static_cast<std::size_t>(node)][i],
                block_weight[static_cast<std::size_t>(node)][i]
            };
            total += block_weight[static_cast<std::size_t>(node)][i];
        }
        auto sift_down = [&](std::size_t root, std::size_t heap_size){
            while(root < heap_size / 2){
                std::size_t child = root * 2 + 1;
                if(child + 1 < heap_size &&
                    buffer[child].first < buffer[child + 1].first
                ) child++;
                if(!(buffer[root].first < buffer[child].first)) break;
                std::swap(buffer[root], buffer[child]);
                root = child;
            }
        };
        for(std::size_t root = size / 2; root > 0; root--) sift_down(root - 1, size);
        for(std::size_t end = size; end > 1; end--){
            std::swap(buffer[0], buffer[end - 1]);
            sift_down(0, end - 1);
        }
        sorted_prefix[static_cast<std::size_t>(node)][0] = W{};
        for(std::size_t i = 0; i < size; i++){
            sorted_value[static_cast<std::size_t>(node)][i] = buffer[i].first;
            sorted_prefix[static_cast<std::size_t>(node)][i + 1] =
                sorted_prefix[static_cast<std::size_t>(node)][i] + buffer[i].second;
        }
        block_sum[static_cast<std::size_t>(node)] = total;
    }
    int copy_block(int old_node){
        int node = new_block();
        int size = block_size[static_cast<std::size_t>(old_node)];
        block_size[static_cast<std::size_t>(node)] = size;
        for(int i = 0; i < size; i++){
            block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)] =
                block_value[static_cast<std::size_t>(old_node)][static_cast<std::size_t>(i)];
            block_weight[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)] =
                block_weight[static_cast<std::size_t>(old_node)][static_cast<std::size_t>(i)];
        }
        return node;
    }
    int block_at(int version, int block) const{
        return block_reference.get(
            version_root[static_cast<std::size_t>(version)], block
        );
    }

    int count_less_encoded(int version, int l, int r, U upper) const{
        int result = 0;
        while(l < r){
            int block = block_index(l);
            int node = block_at(version, block);
            int block_end = std::min(r, (block + 1) * BLOCK_SIZE);
            int first = local_index(l);
            int count = block_end - l;
            if(first == 0 && count == block_size[static_cast<std::size_t>(node)]){
                auto begin = sorted_value[static_cast<std::size_t>(node)].begin();
                result += static_cast<int>(
                    std::lower_bound(begin, begin + count, upper) - begin);
            }else{
                for(int i = 0; i < count; i++){
                    if(block_value[static_cast<std::size_t>(node)][
                        static_cast<std::size_t>(first + i)] < upper
                    ) result++;
                }
            }
            l = block_end;
        }
        return result;
    }

    int count_less_value(int version, int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return r - l;
        return count_less_encoded(version, l, r, key);
    }

    int count_equal_encoded(int version, int l, int r, U key) const{
        int result = 0;
        while(l < r){
            int block = block_index(l);
            int node = block_at(version, block);
            int block_end = std::min(r, (block + 1) * BLOCK_SIZE);
            int first = local_index(l);
            int count = block_end - l;
            if(first == 0 && count == block_size[static_cast<std::size_t>(node)]){
                auto begin = sorted_value[static_cast<std::size_t>(node)].begin();
                auto range = std::equal_range(begin, begin + count, key);
                result += static_cast<int>(range.second - range.first);
            }else{
                for(int i = 0; i < count; i++){
                    if(block_value[static_cast<std::size_t>(node)][
                        static_cast<std::size_t>(first + i)] == key
                    ) result++;
                }
            }
            l = block_end;
        }
        return result;
    }

    W sum_less_encoded(int version, int l, int r, U upper) const{
        W result{};
        while(l < r){
            int block = block_index(l);
            int node = block_at(version, block);
            int block_end = std::min(r, (block + 1) * BLOCK_SIZE);
            int first = local_index(l);
            int count = block_end - l;
            if(first == 0 && count == block_size[static_cast<std::size_t>(node)]){
                auto begin = sorted_value[static_cast<std::size_t>(node)].begin();
                int prefix = static_cast<int>(
                    std::lower_bound(begin, begin + count, upper) - begin);
                result += sorted_prefix[static_cast<std::size_t>(node)][
                    static_cast<std::size_t>(prefix)];
            }else{
                for(int i = 0; i < count; i++){
                    int local = first + i;
                    if(block_value[static_cast<std::size_t>(node)][
                        static_cast<std::size_t>(local)] < upper
                    ){
                        result += block_weight[static_cast<std::size_t>(node)][
                            static_cast<std::size_t>(local)];
                    }
                }
            }
            l = block_end;
        }
        return result;
    }

    W sum_less_value(int version, int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return sum(version, l, r);
        return sum_less_encoded(version, l, r, key);
    }

protected:
    int set_from_version(int version, int k, T value, W weight_value){
        check_version(version, "library assertion fault: range violation (set).");
        check_index(k, "library assertion fault: range violation (set).");
        U encoded = encode_checked(value, "library assertion fault: bit width violation (set).");
        check_version_capacity();
        int block_snapshot = block_nodes;
        int reference_snapshot = block_reference.nodes_used();
        int block = block_index(k);
        try{
            int node = copy_block(block_at(version, block));
            int local = local_index(k);
            block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local)] = encoded;
            block_weight[static_cast<std::size_t>(node)][static_cast<std::size_t>(local)] = weight_value;
            rebuild_block(node);
            int root = block_reference.set(
                version_root[static_cast<std::size_t>(version)], block, node
            );
            int next_version = _versions;
            version_root[static_cast<std::size_t>(next_version)] = root;
            _versions++;
            return next_version;
        }catch(...){
            block_nodes = block_snapshot;
            block_reference.rollback(reference_snapshot);
            throw;
        }
    }
    int fork_from_version(int version){
        check_version(version, "library assertion fault: range violation (fork).");
        check_version_capacity();
        int next_version = _versions;
        version_root[static_cast<std::size_t>(next_version)] =
            version_root[static_cast<std::size_t>(version)];
        _versions++;
        return next_version;
    }

public:
    PersistentWeightedWaveletMatrixBase():
        PersistentWeightedWaveletMatrixBase(std::vector<T>{}, std::vector<W>{}){}
    PersistentWeightedWaveletMatrixBase(const std::vector<T>& sequence, const std::vector<W>& weight_sequence):
        _n(static_cast<int>(sequence.size())), block_count((_n + BLOCK_SIZE - 1) / BLOCK_SIZE), _versions(1){
        if(sequence.size() != weight_sequence.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        if(sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        std::array<int, block_count_max> initial_block{};
        for(int block = 0; block < block_count; block++){
            int node = new_block();
            block_size[static_cast<std::size_t>(node)] = actual_block_size(block);
            int first = block * BLOCK_SIZE;
            for(int i = 0; i < block_size[static_cast<std::size_t>(node)]; i++){
                block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)] = encode_checked(
                    sequence[static_cast<std::size_t>(first + i)],
                    "library assertion fault: bit width violation (constructor)."
                );
                block_weight[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)] =
                    weight_sequence[static_cast<std::size_t>(first + i)];
            }
            rebuild_block(node);
            initial_block[static_cast<std::size_t>(block)] = node;
        }
        version_root[0] = block_reference.build(initial_block, block_count);
    }

    int size() const{ return _n; }
    int versions() const{ return _versions; }
    int latest_version() const{ return _versions - 1; }
    T access(int version, int k) const{
        check_version(version, "library assertion fault: range violation (access).");
        check_index(k, "library assertion fault: range violation (access).");
        int node = block_at(version, block_index(k));
        return decode(block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(k))]);
    }
    W weight(int version, int k) const{
        check_version(version, "library assertion fault: range violation (weight).");
        check_index(k, "library assertion fault: range violation (weight).");
        int node = block_at(version, block_index(k));
        return block_weight[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(k))];
    }
    int rank(int version, T value, int r) const{ return rank(version, value, 0, r); }
    int rank(int version, T value, int l, int r) const{
        check_version(version, "library assertion fault: range violation (rank).");
        check_range(l, r, "library assertion fault: range violation (rank).");
        U key = encode(value);
        if(!fits(key)) return 0;
        return count_equal_encoded(version, l, r, key);
    }
    T kth_smallest(int version, int l, int r, int k) const{
        check_version(version, "library assertion fault: range violation (kth_smallest).");
        check_range(l, r, "library assertion fault: range violation (kth_smallest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_smallest).");
        }
        U value = 0;
        for(int bit = BIT_WIDTH - 1; bit >= 0; bit--){
            U candidate = value | (U{1} << bit);
            if(count_less_encoded(version, l, r, candidate) <= k) value = candidate;
        }
        return decode(value);
    }
    T kth_largest(int version, int l, int r, int k) const{
        check_version(version, "library assertion fault: range violation (kth_largest).");
        check_range(l, r, "library assertion fault: range violation (kth_largest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_largest).");
        }
        return kth_smallest(version, l, r, r - l - 1 - k);
    }
    int range_freq(int version, int l, int r, T upper) const{
        check_version(version, "library assertion fault: range violation (range_freq).");
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        return count_less_value(version, l, r, upper);
    }
    int range_freq(int version, int l, int r, T lower, T upper) const{
        check_version(version, "library assertion fault: range violation (range_freq).");
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        if(upper < lower)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (range_freq).");
        return count_less_value(version, l, r, upper) - count_less_value(version, l, r, lower);
    }
    W sum(int version, int l, int r) const{
        check_version(version, "library assertion fault: range violation (sum).");
        check_range(l, r, "library assertion fault: range violation (sum).");
        W result{};
        while(l < r){
            int block = block_index(l);
            int node = block_at(version, block);
            int block_end = std::min(r, (block + 1) * BLOCK_SIZE);
            int first = local_index(l);
            int count = block_end - l;
            if(first == 0 && count == block_size[static_cast<std::size_t>(node)]){
                result += block_sum[static_cast<std::size_t>(node)];
            }else{
                for(int i = 0; i < count; i++){
                    result += block_weight[static_cast<std::size_t>(node)][
                        static_cast<std::size_t>(first + i)];
                }
            }
            l = block_end;
        }
        return result;
    }
    W range_sum(int version, int l, int r, T upper) const{
        check_version(version, "library assertion fault: range violation (range_sum).");
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        return sum_less_value(version, l, r, upper);
    }
    W range_sum(int version, int l, int r, T lower, T upper) const{
        check_version(version, "library assertion fault: range violation (range_sum).");
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        if(upper < lower)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (range_sum).");
        return sum_less_value(version, l, r, upper) - sum_less_value(version, l, r, lower);
    }
    W sum_k_smallest(int version, int l, int r, int k) const{
        check_version(version, "library assertion fault: range violation (sum_k_smallest).");
        check_range(l, r, "library assertion fault: range violation (sum_k_smallest).");
        if(k < 0 || r - l < k)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (sum_k_smallest).");
        if(k == 0) return W{};
        if(k == r - l) return sum(version, l, r);
        T border = kth_smallest(version, l, r, k - 1);
        W result = range_sum(version, l, r, border);
        int need = k - range_freq(version, l, r, border);
        U key = encode(border);
        while(l < r && need > 0){
            int block = block_index(l);
            int node = block_at(version, block);
            int block_end = std::min(r, (block + 1) * BLOCK_SIZE);
            int first = local_index(l);
            for(int i = 0; i < block_end - l && need > 0; i++){
                int local = first + i;
                if(block_value[static_cast<std::size_t>(node)][
                    static_cast<std::size_t>(local)] == key
                ){
                    result += block_weight[static_cast<std::size_t>(node)][
                        static_cast<std::size_t>(local)];
                    need--;
                }
            }
            l = block_end;
        }
        return result;
    }
    W sum_k_largest(int version, int l, int r, int k) const{
        check_version(version, "library assertion fault: range violation (sum_k_largest).");
        check_range(l, r, "library assertion fault: range violation (sum_k_largest).");
        if(k < 0 || r - l < k)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (sum_k_largest).");
        return sum(version, l, r) - sum_k_smallest(version, l, r, r - l - k);
    }
    std::optional<T> prev_value(int version, int l, int r, T upper) const{
        int count = range_freq(version, l, r, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(version, l, r, count - 1);
    }
    std::optional<T> next_value(int version, int l, int r, T lower) const{
        int count = range_freq(version, l, r, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest(version, l, r, count);
    }
};

} // namespace wavelet_matrix_detail

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PERSISTENT_WEIGHTED_WAVELET_MATRIX_BASE_HPP_INCLUDED
