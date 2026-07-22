#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<
    class T,
    class W,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits,
    int BLOCK_SIZE = 512
>
struct DynamicWeightedWaveletMatrix{
    static_assert(std::is_integral_v<T>);
    static_assert(MAX_SIZE >= 0);
    static_assert(BLOCK_SIZE > 0);
    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;
    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);

private:
    class DynamicWeightedBitVector{
        struct Node{
            bool bit = false;
            W weight{};
            std::uint64_t priority = 0;
            Node* left = nullptr;
            Node* right = nullptr;
            int size = 1;
            int ones = 0;
            W sum{};
            W zero_sum{};
            W one_sum{};

            Node(bool bit_value, const W& weight_value, std::uint64_t priority_value):
                bit(bit_value), weight(weight_value), priority(priority_value),
                ones(bit_value ? 1 : 0), sum(weight_value),
                zero_sum(bit_value ? W{} : weight_value),
                one_sum(bit_value ? weight_value : W{}){}
        };

    public:
        struct Stats{
            int size = 0;
            int ones = 0;
            W sum{};
            W zero_sum{};
            W one_sum{};
        };

    private:
        Node* root = nullptr;
        std::uint64_t random_state = 0x9e3779b97f4a7c15ULL;

        static int node_size(const Node* node){ return node == nullptr ? 0 : node->size; }
        static int node_ones(const Node* node){ return node == nullptr ? 0 : node->ones; }
        static W node_sum(const Node* node){ return node == nullptr ? W{} : node->sum; }
        static W node_zero_sum(const Node* node){ return node == nullptr ? W{} : node->zero_sum; }
        static W node_one_sum(const Node* node){ return node == nullptr ? W{} : node->one_sum; }

        static void pull(Node* node){
            if(node == nullptr) return;
            node->size = 1 + node_size(node->left) + node_size(node->right);
            node->ones = (node->bit ? 1 : 0) + node_ones(node->left) + node_ones(node->right);
            node->sum = node_sum(node->left) + node->weight + node_sum(node->right);
            node->zero_sum = node_zero_sum(node->left) +
                (node->bit ? W{} : node->weight) + node_zero_sum(node->right);
            node->one_sum = node_one_sum(node->left) +
                (node->bit ? node->weight : W{}) + node_one_sum(node->right);
        }

        std::uint64_t next_priority(){
            random_state += 0x9e3779b97f4a7c15ULL;
            std::uint64_t value = random_state;
            value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
            value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
            return value ^ (value >> 31);
        }

        static std::pair<Node*, Node*> split(Node* node, int left_size){
            if(node == nullptr) return {nullptr, nullptr};
            if(node_size(node->left) >= left_size){
                auto [left, middle] = split(node->left, left_size);
                node->left = middle;
                pull(node);
                return {left, node};
            }
            auto [middle, right] = split(
                node->right, left_size - node_size(node->left) - 1
            );
            node->right = middle;
            pull(node);
            return {node, right};
        }

        static Node* merge(Node* left, Node* right){
            if(left == nullptr) return right;
            if(right == nullptr) return left;
            if(left->priority > right->priority){
                left->right = merge(left->right, right);
                pull(left);
                return left;
            }
            right->left = merge(left, right->left);
            pull(right);
            return right;
        }

        static void destroy(Node* node){
            if(node == nullptr) return;
            destroy(node->left);
            destroy(node->right);
            delete node;
        }

        static Node* clone(const Node* node){
            if(node == nullptr) return nullptr;
            Node* result = new Node(node->bit, node->weight, node->priority);
            result->left = clone(node->left);
            result->right = clone(node->right);
            pull(result);
            return result;
        }

        static void pull_all(Node* node){
            if(node == nullptr) return;
            pull_all(node->left);
            pull_all(node->right);
            pull(node);
        }

        static void add_subtree(Stats& result, const Node* node){
            if(node == nullptr) return;
            result.size += node->size;
            result.ones += node->ones;
            result.sum += node->sum;
            result.zero_sum += node->zero_sum;
            result.one_sum += node->one_sum;
        }

        Stats prefix_stats(int end) const{
            Stats result;
            const Node* node = root;
            while(node != nullptr && end > 0){
                const int left_size = node_size(node->left);
                if(end <= left_size){
                    node = node->left;
                    continue;
                }
                add_subtree(result, node->left);
                result.size++;
                result.ones += node->bit ? 1 : 0;
                result.sum += node->weight;
                if(node->bit) result.one_sum += node->weight;
                else result.zero_sum += node->weight;
                end -= left_size + 1;
                node = node->right;
            }
            return result;
        }

        static bool set_weight_recursive(Node* node, int position, const W& value){
            const int left_size = node_size(node->left);
            bool result;
            if(position < left_size){
                result = set_weight_recursive(node->left, position, value);
            }else if(position == left_size){
                node->weight = value;
                result = node->bit;
            }else{
                result = set_weight_recursive(node->right, position - left_size - 1, value);
            }
            pull(node);
            return result;
        }

    public:
        DynamicWeightedBitVector() = default;

        DynamicWeightedBitVector(const DynamicWeightedBitVector& other):
            root(clone(other.root)), random_state(other.random_state){}

        DynamicWeightedBitVector& operator=(const DynamicWeightedBitVector& other){
            if(this == &other) return *this;
            Node* copied = clone(other.root);
            destroy(root);
            root = copied;
            random_state = other.random_state;
            return *this;
        }

        DynamicWeightedBitVector(DynamicWeightedBitVector&& other) noexcept:
            root(other.root), random_state(other.random_state){
            other.root = nullptr;
        }

        DynamicWeightedBitVector& operator=(DynamicWeightedBitVector&& other) noexcept{
            if(this == &other) return *this;
            destroy(root);
            root = other.root;
            random_state = other.random_state;
            other.root = nullptr;
            return *this;
        }

        ~DynamicWeightedBitVector(){ destroy(root); }

        int size() const{ return node_size(root); }
        int ones() const{ return node_ones(root); }

        void assign(const std::vector<unsigned char>& bits, const std::vector<W>& weights){
            destroy(root);
            root = nullptr;
            std::vector<Node*> stack;
            stack.reserve(bits.size());
            for(std::size_t i = 0; i < bits.size(); i++){
                Node* current = new Node(bits[i] != 0, weights[i], next_priority());
                Node* last = nullptr;
                while(!stack.empty() && stack.back()->priority < current->priority){
                    last = stack.back();
                    stack.pop_back();
                }
                current->left = last;
                if(stack.empty()) root = current;
                else stack.back()->right = current;
                stack.push_back(current);
            }
            pull_all(root);
        }

        std::pair<bool, W> access(int position) const{
            const Node* node = root;
            while(node != nullptr){
                const int left_size = node_size(node->left);
                if(position < left_size) node = node->left;
                else if(position == left_size) return {node->bit, node->weight};
                else{
                    position -= left_size + 1;
                    node = node->right;
                }
            }
            throw std::logic_error("DynamicWeightedBitVector access failure.");
        }

        Stats range_stats(int left, int right) const{
            Stats lhs = prefix_stats(left);
            Stats rhs = prefix_stats(right);
            rhs.size -= lhs.size;
            rhs.ones -= lhs.ones;
            rhs.sum = rhs.sum - lhs.sum;
            rhs.zero_sum = rhs.zero_sum - lhs.zero_sum;
            rhs.one_sum = rhs.one_sum - lhs.one_sum;
            return rhs;
        }

        int rank(bool value, int end) const{
            Stats stats = prefix_stats(end);
            return value ? stats.ones : end - stats.ones;
        }

        int select(bool value, int kth) const{
            if(kth < 0) return size();
            const int total = value ? ones() : size() - ones();
            if(total <= kth) return size();
            const Node* node = root;
            int offset = 0;
            while(node != nullptr){
                const int left_count = value
                    ? node_ones(node->left)
                    : node_size(node->left) - node_ones(node->left);
                if(kth < left_count){
                    node = node->left;
                    continue;
                }
                kth -= left_count;
                const int left_size = node_size(node->left);
                if(node->bit == value){
                    if(kth == 0) return offset + left_size;
                    kth--;
                }
                offset += left_size + 1;
                node = node->right;
            }
            return size();
        }

        W weight_of_first(bool value, int count) const{
            W result{};
            const Node* node = root;
            while(node != nullptr && count > 0){
                const int left_count = value
                    ? node_ones(node->left)
                    : node_size(node->left) - node_ones(node->left);
                if(count <= left_count){
                    node = node->left;
                    continue;
                }
                result += value ? node_one_sum(node->left) : node_zero_sum(node->left);
                count -= left_count;
                if(node->bit == value){
                    result += node->weight;
                    if(--count == 0) break;
                }
                node = node->right;
            }
            return result;
        }

        W weight_of_first(bool value, int left, int count) const{
            const int skipped = rank(value, left);
            return weight_of_first(value, skipped + count) - weight_of_first(value, skipped);
        }

        void insert(int position, bool value, const W& weight){
            auto [left, right] = split(root, position);
            Node* middle = new Node(value, weight, next_priority());
            root = merge(merge(left, middle), right);
        }

        std::pair<bool, W> erase(int position){
            auto [left, suffix] = split(root, position);
            auto [middle, right] = split(suffix, 1);
            if(middle == nullptr) throw std::logic_error("DynamicWeightedBitVector erase failure.");
            std::pair<bool, W> result{middle->bit, middle->weight};
            middle->left = nullptr;
            middle->right = nullptr;
            delete middle;
            root = merge(left, right);
            return result;
        }

        bool set_weight(int position, const W& value){
            return set_weight_recursive(root, position, value);
        }
    };

    int _n = 0;
    std::array<DynamicWeightedBitVector, static_cast<std::size_t>(BIT_WIDTH)> bit_vectors{};
    std::array<int, static_cast<std::size_t>(BIT_WIDTH)> zero_count{};

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

    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]] throw std::runtime_error(message);
    }

    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]] throw std::runtime_error(message);
    }

    U access_encoded(int k) const{
        U value = 0;
        int position = k;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto [bit, ignored_weight] =
                bit_vectors[static_cast<std::size_t>(level)].access(position);
            (void)ignored_weight;
            value = static_cast<U>((value << 1) | (bit ? U{1} : U{0}));
            const int ones_before =
                bit_vectors[static_cast<std::size_t>(level)].rank(true, position);
            position = bit
                ? zero_count[static_cast<std::size_t>(level)] + ones_before
                : position - ones_before;
        }
        return value;
    }

    int count_less_encoded(int l, int r, U upper) const{
        int result = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const bool bit =
                static_cast<bool>((upper >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = ones_l + stats.ones;
            if(bit){
                result += zeros;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r = l + zeros;
            }
        }
        return result;
    }

    int count_less_value(int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return r - l;
        return count_less_encoded(l, r, key);
    }

    W sum_less_encoded(int l, int r, U upper) const{
        W result{};
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const bool bit =
                static_cast<bool>((upper >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = ones_l + stats.ones;
            if(bit){
                result += stats.zero_sum;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r = l + zeros;
            }
        }
        return result;
    }

    W sum_less_value(int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return sum(l, r);
        return sum_less_encoded(l, r, key);
    }

public:
    DynamicWeightedWaveletMatrix() = default;

    DynamicWeightedWaveletMatrix(
        const std::vector<T>& sequence,
        const std::vector<W>& weight_sequence
    ): _n(static_cast<int>(sequence.size())){
        if(sequence.size() != weight_sequence.size() ||
           sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        std::vector<U> current_value(sequence.size());
        std::vector<W> current_weight = weight_sequence;
        for(std::size_t i = 0; i < sequence.size(); i++){
            current_value[i] = encode_checked(
                sequence[i], "library assertion fault: bit width violation (constructor)."
            );
        }
        for(int level = 0; level < BIT_WIDTH; level++){
            const int shift = BIT_WIDTH - 1 - level;
            std::vector<unsigned char> bits(current_value.size());
            std::vector<U> next_value;
            std::vector<W> next_weight;
            next_value.reserve(current_value.size());
            next_weight.reserve(current_weight.size());
            for(std::size_t i = 0; i < current_value.size(); i++){
                bits[i] =
                    static_cast<unsigned char>((current_value[i] >> shift) & U{1});
            }
            bit_vectors[static_cast<std::size_t>(level)].assign(bits, current_weight);
            for(int target = 0; target < 2; target++){
                for(std::size_t i = 0; i < current_value.size(); i++){
                    if(bits[i] == target){
                        next_value.push_back(current_value[i]);
                        next_weight.push_back(current_weight[i]);
                    }
                }
                if(target == 0){
                    zero_count[static_cast<std::size_t>(level)] =
                        static_cast<int>(next_value.size());
                }
            }
            current_value.swap(next_value);
            current_weight.swap(next_weight);
        }
    }

    template<std::size_t N>
    DynamicWeightedWaveletMatrix(
        const std::array<T, N>& sequence,
        const std::array<W, N>& weight_sequence
    ): DynamicWeightedWaveletMatrix(
        std::vector<T>(sequence.begin(), sequence.end()),
        std::vector<W>(weight_sequence.begin(), weight_sequence.end())
    ){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int size() const{ return _n; }

    T access(int k) const{
        check_index(k, "library assertion fault: range violation (access).");
        return decode(access_encoded(k));
    }
    T operator[](int k) const{ return access(k); }

    W weight(int k) const{
        check_index(k, "library assertion fault: range violation (weight).");
        return bit_vectors[0].access(k).second;
    }

    void insert(int position, T value, const W& weight_value){
        if(position < 0 || _n < position || _n == MAX_SIZE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (insert).");
        }
        U encoded = encode_checked(
            value, "library assertion fault: bit width violation (insert)."
        );
        int current = position;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const int ones_before = bits.rank(true, current);
            const int old_zeros = zero_count[static_cast<std::size_t>(level)];
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            bits.insert(current, bit, weight_value);
            if(bit){
                current = old_zeros + ones_before;
            }else{
                current -= ones_before;
                zero_count[static_cast<std::size_t>(level)]++;
            }
        }
        _n++;
    }

    std::pair<T, W> erase(int position){
        check_index(position, "library assertion fault: range violation (erase).");
        U encoded = 0;
        W erased_weight{};
        int current = position;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto [bit, weight_value] = bits.access(current);
            if(level == 0) erased_weight = weight_value;
            encoded = static_cast<U>((encoded << 1) | (bit ? U{1} : U{0}));
            const int ones_before = bits.rank(true, current);
            const int old_zeros = zero_count[static_cast<std::size_t>(level)];
            const int next =
                bit ? old_zeros + ones_before : current - ones_before;
            bits.erase(current);
            if(!bit) zero_count[static_cast<std::size_t>(level)]--;
            current = next;
        }
        _n--;
        return {decode(encoded), erased_weight};
    }

    void push_back(T value, const W& weight_value){
        insert(_n, value, weight_value);
    }

    std::pair<T, W> pop_back(){
        if(_n == 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (pop_back).");
        }
        return erase(_n - 1);
    }

    void set(int k, T value, const W& weight_value){
        check_index(k, "library assertion fault: range violation (set).");
        encode_checked(value, "library assertion fault: bit width violation (set).");
        erase(k);
        insert(k, value, weight_value);
    }

    void set_value(int k, T value){ set(k, value, weight(k)); }

    void set_weight(int k, const W& weight_value){
        check_index(k, "library assertion fault: range violation (set_weight).");
        int current = k;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const int ones_before = bits.rank(true, current);
            const bool bit = bits.set_weight(current, weight_value);
            current = bit
                ? zero_count[static_cast<std::size_t>(level)] + ones_before
                : current - ones_before;
        }
    }

    int rank(T value, int r) const{ return rank(value, 0, r); }

    int rank(T value, int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (rank).");
        U encoded = encode(value);
        if(!fits(encoded)) return 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = bits.rank(true, r);
            if(bit){
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r -= ones_r;
            }
        }
        return r - l;
    }

    int select(T value, int k) const{
        if(k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (select).");
        }
        U encoded = encode(value);
        if(!fits(encoded)) return _n;
        int l = 0, r = _n;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = bits.rank(true, r);
            if(bit){
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r -= ones_r;
            }
        }
        if(r - l <= k) return _n;
        int position = l + k;
        for(int level = BIT_WIDTH - 1; level >= 0; level--){
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            position = bit_vectors[static_cast<std::size_t>(level)].select(
                bit,
                bit ? position - zero_count[static_cast<std::size_t>(level)] : position
            );
        }
        return position;
    }

    T kth_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_smallest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_smallest)."
            );
        }
        U value = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const int ones_l = bits.rank(true, l);
            if(k < zeros){
                value <<= 1;
                l -= ones_l;
                r = l + zeros;
            }else{
                value = static_cast<U>((value << 1) | U{1});
                k -= zeros;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = l + stats.ones;
            }
        }
        return decode(value);
    }

    T kth_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_largest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_largest)."
            );
        }
        return kth_smallest(l, r, r - l - 1 - k);
    }

    int range_freq(int l, int r, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        return count_less_value(l, r, upper);
    }

    int range_freq(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (range_freq)."
            );
        }
        return count_less_value(l, r, upper) - count_less_value(l, r, lower);
    }

    W sum(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (sum).");
        return bit_vectors[0].range_stats(l, r).sum;
    }

    W range_sum(int l, int r, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        return sum_less_value(l, r, upper);
    }

    W range_sum(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (range_sum)."
            );
        }
        return sum_less_value(l, r, upper) - sum_less_value(l, r, lower);
    }

    W sum_k_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_smallest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (sum_k_smallest)."
            );
        }
        W result{};
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const int ones_l = bits.rank(true, l);
            if(level == BIT_WIDTH - 1){
                if(k <= zeros){
                    result += bits.weight_of_first(false, l, k);
                }else{
                    result += stats.zero_sum;
                    result += bits.weight_of_first(true, l, k - zeros);
                }
                return result;
            }
            if(k <= zeros){
                l -= ones_l;
                r = l + zeros;
            }else{
                result += stats.zero_sum;
                k -= zeros;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = l + stats.ones;
            }
        }
        return result;
    }

    W sum_k_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_largest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (sum_k_largest)."
            );
        }
        return sum(l, r) - sum_k_smallest(l, r, r - l - k);
    }

    std::optional<T> prev_value(int l, int r, T upper) const{
        int count = range_freq(l, r, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(l, r, count - 1);
    }

    std::optional<T> next_value(int l, int r, T lower) const{
        int count = range_freq(l, r, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest(l, r, count);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
