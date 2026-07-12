#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<
    class T,
    int MAX_SIZE,
    int MAX_VERSION,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits,
    int BLOCK_SIZE = 512
>
struct PartiallyPersistentWaveletMatrix{
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
    std::array<std::array<int, block_count_max>, MAX_VERSION + 1> version_block{};
    std::array<std::array<U, BLOCK_SIZE>, block_node_max> block_value{};
    std::array<std::array<U, BLOCK_SIZE>, block_node_max> block_sorted{};
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
        if(_versions > MAX_VERSION + 1)[[unlikely]]{
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
        for(int i = 0; i < block_size[static_cast<std::size_t>(node)]; i++){
            block_sorted[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)] =
                block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)];
        }
        auto begin = block_sorted[static_cast<std::size_t>(node)].begin();
        std::sort(begin, begin + block_size[static_cast<std::size_t>(node)]);
    }

    int copy_block(int old_node){
        int node = new_block();
        block_size[static_cast<std::size_t>(node)] = block_size[static_cast<std::size_t>(old_node)];
        for(int i = 0; i < block_size[static_cast<std::size_t>(node)]; i++){
            block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)] =
                block_value[static_cast<std::size_t>(old_node)][static_cast<std::size_t>(i)];
        }
        return node;
    }

    void copy_version_blocks(int from, int to){
        for(int block = 0; block < block_count; block++){
            version_block[static_cast<std::size_t>(to)][static_cast<std::size_t>(block)] =
                version_block[static_cast<std::size_t>(from)][static_cast<std::size_t>(block)];
        }
    }

    int count_less_encoded(int version, int l, int r, U upper) const{
        int result = 0;
        while(l < r && l % BLOCK_SIZE != 0){
            int node = version_block[static_cast<std::size_t>(version)][
                static_cast<std::size_t>(block_index(l))];
            if(block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(l))] < upper){
                result++;
            }
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            int node = version_block[static_cast<std::size_t>(version)][
                static_cast<std::size_t>(block_index(l))];
            auto begin = block_sorted[static_cast<std::size_t>(node)].begin();
            int size = block_size[static_cast<std::size_t>(node)];
            result += static_cast<int>(std::lower_bound(begin, begin + size, upper) - begin);
            l += BLOCK_SIZE;
        }
        while(l < r){
            int node = version_block[static_cast<std::size_t>(version)][
                static_cast<std::size_t>(block_index(l))];
            if(block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(l))] < upper){
                result++;
            }
            l++;
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
        while(l < r && l % BLOCK_SIZE != 0){
            int node = version_block[static_cast<std::size_t>(version)][
                static_cast<std::size_t>(block_index(l))];
            if(block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(l))] == key){
                result++;
            }
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            int node = version_block[static_cast<std::size_t>(version)][
                static_cast<std::size_t>(block_index(l))];
            auto begin = block_sorted[static_cast<std::size_t>(node)].begin();
            int size = block_size[static_cast<std::size_t>(node)];
            auto range = std::equal_range(begin, begin + size, key);
            result += static_cast<int>(range.second - range.first);
            l += BLOCK_SIZE;
        }
        while(l < r){
            int node = version_block[static_cast<std::size_t>(version)][
                static_cast<std::size_t>(block_index(l))];
            if(block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(l))] == key){
                result++;
            }
            l++;
        }
        return result;
    }

public:
    PartiallyPersistentWaveletMatrix() = default;

    explicit PartiallyPersistentWaveletMatrix(const std::vector<T>& sequence):
        _n(static_cast<int>(sequence.size())),
        block_count((_n + BLOCK_SIZE - 1) / BLOCK_SIZE),
        _versions(1){
        if(sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int block = 0; block < block_count; block++){
            int node = new_block();
            block_size[static_cast<std::size_t>(node)] = actual_block_size(block);
            int first = block * BLOCK_SIZE;
            for(int i = 0; i < block_size[static_cast<std::size_t>(node)]; i++){
                block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(i)] =
                    encode_checked(
                        sequence[static_cast<std::size_t>(first + i)],
                        "library assertion fault: bit width violation (constructor)."
                    );
            }
            rebuild_block(node);
            version_block[0][static_cast<std::size_t>(block)] = node;
        }
    }

    template<std::size_t N>
    explicit PartiallyPersistentWaveletMatrix(const std::array<T, N>& sequence):
        PartiallyPersistentWaveletMatrix(std::vector<T>(sequence.begin(), sequence.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int size() const{ return _n; }
    int versions() const{ return _versions; }
    int latest_version() const{ return _versions - 1; }

    T access(int version, int k) const{
        check_version(version, "library assertion fault: range violation (access).");
        check_index(k, "library assertion fault: range violation (access).");
        int node = version_block[static_cast<std::size_t>(version)][
            static_cast<std::size_t>(block_index(k))];
        return decode(block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(k))]);
    }

    int set(int k, T value){
        check_index(k, "library assertion fault: range violation (set).");
        U encoded = encode_checked(value, "library assertion fault: bit width violation (set).");
        int base_version = _versions - 1;
        _versions++;
        check_version_capacity();
        int next_version = _versions - 1;
        copy_version_blocks(base_version, next_version);
        int block = block_index(k);
        int old_node = version_block[static_cast<std::size_t>(base_version)][static_cast<std::size_t>(block)];
        int node = copy_block(old_node);
        block_value[static_cast<std::size_t>(node)][static_cast<std::size_t>(local_index(k))] = encoded;
        rebuild_block(node);
        version_block[static_cast<std::size_t>(next_version)][static_cast<std::size_t>(block)] = node;
        return next_version;
    }


    int rank(int version, T value, int r) const{ return rank(version, value, 0, r); }
    int rank(int version, T value, int l, int r) const{
        check_version(version, "library assertion fault: range violation (rank).");
        check_range(l, r, "library assertion fault: range violation (rank).");
        U key = encode(value);
        if(!fits(key)) return 0;
        return count_equal_encoded(version, l, r, key);
    }

    int select(int version, T value, int k) const{
        check_version(version, "library assertion fault: range violation (select).");
        if(k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (select).");
        }
        if(rank(version, value, _n) <= k) return _n;
        int low = 0, high = _n;
        while(low < high){
            int mid = low + (high - low) / 2;
            if(rank(version, value, mid + 1) <= k) low = mid + 1;
            else high = mid;
        }
        return low;
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
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_freq).");
        }
        return count_less_value(version, l, r, upper) - count_less_value(version, l, r, lower);
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
