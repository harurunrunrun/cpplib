#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

template<class T, class Hash = std::hash<T>>
struct RangeMajorityQuery{
private:
    struct Candidate{
        int representative = -1;
        int balance = 0;
    };
    struct PositionNode{
        int left = -1;
        int right = -1;
        int size = 1;
        std::uint64_t priority = 0;
    };

    int length = 0;
    int segment_base = 1;
    std::vector<T> values;
    std::vector<Candidate> segment;
    std::vector<PositionNode> positions;
    std::unordered_map<T, int, Hash> roots;

    static std::uint64_t splitmix64(std::uint64_t value){
        value += 0x9e3779b97f4a7c15ULL;
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }

    int node_size(int node) const{
        return node == -1 ? 0 : positions[static_cast<std::size_t>(node)].size;
    }
    void pull(int node){
        auto& current = positions[static_cast<std::size_t>(node)];
        current.size = 1 + node_size(current.left) + node_size(current.right);
    }
    int merge_positions(int left, int right){
        if(left == -1) return right;
        if(right == -1) return left;
        if(positions[static_cast<std::size_t>(left)].priority <
           positions[static_cast<std::size_t>(right)].priority){
            positions[static_cast<std::size_t>(left)].right = merge_positions(
                positions[static_cast<std::size_t>(left)].right, right
            );
            pull(left);
            return left;
        }
        positions[static_cast<std::size_t>(right)].left = merge_positions(
            left, positions[static_cast<std::size_t>(right)].left
        );
        pull(right);
        return right;
    }
    std::pair<int, int> split_positions(int root, int key){
        if(root == -1) return {-1, -1};
        if(root < key){
            auto [left, right] = split_positions(
                positions[static_cast<std::size_t>(root)].right, key
            );
            positions[static_cast<std::size_t>(root)].right = left;
            pull(root);
            return {root, right};
        }
        auto [left, right] = split_positions(
            positions[static_cast<std::size_t>(root)].left, key
        );
        positions[static_cast<std::size_t>(root)].left = right;
        pull(root);
        return {left, root};
    }
    int insert_position(int root, int node){
        if(root == -1) return node;
        if(positions[static_cast<std::size_t>(node)].priority <
           positions[static_cast<std::size_t>(root)].priority){
            auto [left, right] = split_positions(root, node);
            auto& inserted = positions[static_cast<std::size_t>(node)];
            inserted.left = left;
            inserted.right = right;
            pull(node);
            return node;
        }
        if(node < root){
            positions[static_cast<std::size_t>(root)].left = insert_position(
                positions[static_cast<std::size_t>(root)].left, node
            );
        }else{
            positions[static_cast<std::size_t>(root)].right = insert_position(
                positions[static_cast<std::size_t>(root)].right, node
            );
        }
        pull(root);
        return root;
    }
    int erase_position(int root, int key){
        if(root == -1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: inconsistent state (RangeMajorityQuery)."
            );
        }
        if(root == key){
            auto& erased = positions[static_cast<std::size_t>(root)];
            const int result = merge_positions(erased.left, erased.right);
            erased.left = erased.right = -1;
            erased.size = 1;
            return result;
        }
        if(key < root){
            positions[static_cast<std::size_t>(root)].left = erase_position(
                positions[static_cast<std::size_t>(root)].left, key
            );
        }else{
            positions[static_cast<std::size_t>(root)].right = erase_position(
                positions[static_cast<std::size_t>(root)].right, key
            );
        }
        pull(root);
        return root;
    }
    int count_less(int root, int key) const{
        int result = 0;
        while(root != -1){
            const auto& current = positions[static_cast<std::size_t>(root)];
            if(root < key){
                result += node_size(current.left) + 1;
                root = current.right;
            }else{
                root = current.left;
            }
        }
        return result;
    }

    Candidate merge_candidate(Candidate left, Candidate right) const{
        if(left.balance == 0) return right;
        if(right.balance == 0) return left;
        if(values[static_cast<std::size_t>(left.representative)] ==
           values[static_cast<std::size_t>(right.representative)]){
            left.balance += right.balance;
            return left;
        }
        if(left.balance > right.balance){
            left.balance -= right.balance;
            return left;
        }
        if(right.balance > left.balance){
            right.balance -= left.balance;
            return right;
        }
        return {};
    }
    void rebuild_segment(int index){
        int node = segment_base + index;
        segment[static_cast<std::size_t>(node)] = {index, 1};
        while((node >>= 1) != 0){
            segment[static_cast<std::size_t>(node)] = merge_candidate(
                segment[static_cast<std::size_t>(node << 1)],
                segment[static_cast<std::size_t>(node << 1 | 1)]
            );
        }
    }
    void check_index(int index) const{
        if(index < 0 || length <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (RangeMajorityQuery::set)."
            );
        }
    }
    void check_range(int left, int right) const{
        if(left < 0 || right < left || length < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (RangeMajorityQuery::majority)."
            );
        }
    }

public:
    RangeMajorityQuery() = default;

    explicit RangeMajorityQuery(std::vector<T> sequence):
        length(static_cast<int>(sequence.size())),
        values(std::move(sequence)),
        positions(static_cast<std::size_t>(length)){
        while(segment_base < length) segment_base <<= 1;
        segment.assign(static_cast<std::size_t>(segment_base << 1), {});
        roots.reserve(static_cast<std::size_t>(length) * 2 + 1);
        for(int index = 0; index < length; ++index){
            auto& node = positions[static_cast<std::size_t>(index)];
            node.priority = splitmix64(static_cast<std::uint64_t>(index));
            auto [iterator, inserted] = roots.try_emplace(
                values[static_cast<std::size_t>(index)], -1
            );
            (void)inserted;
            iterator->second = insert_position(iterator->second, index);
            segment[static_cast<std::size_t>(segment_base + index)] = {index, 1};
        }
        for(int node = segment_base - 1; node > 0; --node){
            segment[static_cast<std::size_t>(node)] = merge_candidate(
                segment[static_cast<std::size_t>(node << 1)],
                segment[static_cast<std::size_t>(node << 1 | 1)]
            );
        }
    }

    int size() const{ return length; }

    const T& get(int index) const{
        check_index(index);
        return values[static_cast<std::size_t>(index)];
    }

    void set(int index, const T& value){
        check_index(index);
        T& current = values[static_cast<std::size_t>(index)];
        if(current == value) return;

        auto old_iterator = roots.find(current);
        old_iterator->second = erase_position(old_iterator->second, index);
        auto [new_iterator, inserted] = roots.try_emplace(value, -1);
        (void)inserted;
        new_iterator->second = insert_position(new_iterator->second, index);
        current = value;
        rebuild_segment(index);
    }

    std::optional<T> majority(int left, int right) const{
        check_range(left, right);
        if(left == right) return std::nullopt;
        Candidate left_result;
        Candidate right_result;
        int l = left + segment_base;
        int r = right + segment_base;
        while(l < r){
            if((l & 1) != 0){
                left_result = merge_candidate(
                    left_result, segment[static_cast<std::size_t>(l++)]
                );
            }
            if((r & 1) != 0){
                right_result = merge_candidate(
                    segment[static_cast<std::size_t>(--r)], right_result
                );
            }
            l >>= 1;
            r >>= 1;
        }
        const Candidate candidate = merge_candidate(left_result, right_result);
        if(candidate.balance == 0) return std::nullopt;
        const T& value = values[static_cast<std::size_t>(candidate.representative)];
        const auto iterator = roots.find(value);
        const int frequency = count_less(iterator->second, right) -
            count_less(iterator->second, left);
        if((right - left) / 2 < frequency) return value;
        return std::nullopt;
    }
};
