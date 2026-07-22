#ifndef CPPLIB_SRC_APPROXIMATE_PACKING_NUMBER_PARTITIONING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_PACKING_NUMBER_PARTITIONING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::packing {

template<class Weight>
struct PartitionResult{
    std::vector<std::size_t> part_of_item;
    std::vector<Weight> part_sum;

    [[nodiscard]] std::size_t part_count() const noexcept{
        return part_sum.size();
    }

    [[nodiscard]] Weight maximum_part_sum() const{
        if(part_sum.empty()) return Weight{};
        return *std::max_element(part_sum.begin(), part_sum.end());
    }
};

namespace internal {

template<class Weight>
void validate_partition_input(
    const std::vector<Weight>& item_weight,
    std::size_t part_count
){
    if(part_count == 0)[[unlikely]]{
        throw std::invalid_argument("part count must be positive");
    }
    for(const Weight& weight: item_weight){
        if(weight < Weight{})[[unlikely]]{
            throw std::invalid_argument("item weight must be nonnegative");
        }
        if constexpr(std::is_floating_point_v<Weight>){
            if(!std::isfinite(weight))[[unlikely]]{
                throw std::invalid_argument("item weight must be finite");
            }
        }
    }
}

template<class Weight>
void checked_add_nonnegative(Weight& destination, const Weight& value){
    if constexpr(
        std::numeric_limits<Weight>::is_integer
        && std::numeric_limits<Weight>::is_bounded
    ){
        if(std::numeric_limits<Weight>::max() - destination < value)[[unlikely]]{
            throw std::overflow_error("partition sum overflow");
        }
        destination += value;
    }else{
        const Weight sum = destination + value;
        if constexpr(std::is_floating_point_v<Weight>){
            if(!std::isfinite(sum))[[unlikely]]{
                throw std::overflow_error("partition sum overflow");
            }
        }
        destination = sum;
    }
}

template<class Weight>
std::vector<std::size_t> partition_order(
    const std::vector<Weight>& item_weight,
    bool decreasing
){
    std::vector<std::size_t> order(item_weight.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    if(decreasing){
        std::stable_sort(
            order.begin(),
            order.end(),
            [&](std::size_t first, std::size_t second){
                return item_weight[second] < item_weight[first];
            }
        );
    }
    return order;
}

template<class Weight>
struct PartLoad{
    Weight sum;
    std::size_t part;
};

template<class Weight>
struct GreaterPartLoad{
    bool operator()(const PartLoad<Weight>& first, const PartLoad<Weight>& second) const{
        if(first.sum != second.sum) return second.sum < first.sum;
        return second.part < first.part;
    }
};

}  // namespace internal

template<class Weight>
PartitionResult<Weight> greedy_load_balancing(
    const std::vector<Weight>& item_weight,
    std::size_t part_count,
    bool decreasing = false
){
    internal::validate_partition_input(item_weight, part_count);
    PartitionResult<Weight> result;
    result.part_of_item.resize(item_weight.size());
    result.part_sum.assign(part_count, Weight{});

    std::priority_queue<
        internal::PartLoad<Weight>,
        std::vector<internal::PartLoad<Weight>>,
        internal::GreaterPartLoad<Weight>
    > queue;
    for(std::size_t part = 0; part < part_count; ++part){
        queue.push({Weight{}, part});
    }
    for(const std::size_t item:
        internal::partition_order(item_weight, decreasing)){
        const auto current = queue.top();
        queue.pop();
        result.part_of_item[item] = current.part;
        internal::checked_add_nonnegative(
            result.part_sum[current.part],
            item_weight[item]
        );
        queue.push({result.part_sum[current.part], current.part});
    }
    return result;
}

template<class Weight>
PartitionResult<Weight> greedy_multiway_partition(
    const std::vector<Weight>& item_weight,
    std::size_t part_count
){
    return greedy_load_balancing(item_weight, part_count, true);
}

template<class Weight>
PartitionResult<Weight> greedy_number_partition(
    const std::vector<Weight>& item_weight
){
    return greedy_load_balancing(item_weight, 2, true);
}
template<class Weight>
PartitionResult<Weight> greedy_two_way_partition(
    const std::vector<Weight>& item_weight
){
    return greedy_number_partition(item_weight);
}


template<class Weight>
PartitionResult<Weight> round_robin_partition(
    const std::vector<Weight>& item_weight,
    std::size_t part_count,
    bool decreasing = false
){
    internal::validate_partition_input(item_weight, part_count);
    PartitionResult<Weight> result;
    result.part_of_item.resize(item_weight.size());
    result.part_sum.assign(part_count, Weight{});
    std::size_t position = 0;
    for(const std::size_t item:
        internal::partition_order(item_weight, decreasing)){
        const std::size_t part = position % part_count;
        ++position;
        result.part_of_item[item] = part;
        internal::checked_add_nonnegative(result.part_sum[part], item_weight[item]);
    }
    return result;
}

template<class Weight>
PartitionResult<Weight> largest_differencing_partition(
    const std::vector<Weight>& item_weight
){
    internal::validate_partition_input(item_weight, 2);

    struct Node{
        std::size_t left;
        std::size_t right;
        std::size_t item;
        bool is_leaf;
    };
    struct Entry{
        Weight difference;
        std::size_t node;
    };
    struct LessEntry{
        bool operator()(const Entry& first, const Entry& second) const{
            if(first.difference != second.difference){
                return first.difference < second.difference;
            }
            return second.node < first.node;
        }
    };

    PartitionResult<Weight> result;
    result.part_of_item.resize(item_weight.size());
    result.part_sum.assign(2, Weight{});
    if(item_weight.empty()) return result;

    if(item_weight.size() > std::numeric_limits<std::size_t>::max() / 2U + 1U){
        throw std::length_error("too many items for differencing tree");
    }

    std::vector<Node> nodes;
    nodes.reserve(item_weight.size() + (item_weight.size() - 1U));
    std::priority_queue<Entry, std::vector<Entry>, LessEntry> queue;
    for(std::size_t item = 0; item < item_weight.size(); ++item){
        nodes.push_back({0, 0, item, true});
        queue.push({item_weight[item], item});
    }
    while(queue.size() > 1){
        const Entry first = queue.top();
        queue.pop();
        const Entry second = queue.top();
        queue.pop();
        const std::size_t node = nodes.size();
        nodes.push_back({first.node, second.node, 0, false});
        queue.push({first.difference - second.difference, node});
    }

    std::vector<std::pair<std::size_t, bool>> stack;
    stack.reserve(nodes.size());
    stack.emplace_back(queue.top().node, false);
    while(!stack.empty()){
        const auto [node_index, reversed] = stack.back();
        stack.pop_back();
        const Node& node = nodes[node_index];
        if(node.is_leaf){
            result.part_of_item[node.item] = reversed ? 1U : 0U;
            continue;
        }
        stack.emplace_back(node.right, !reversed);
        stack.emplace_back(node.left, reversed);
    }
    for(std::size_t item = 0; item < item_weight.size(); ++item){
        internal::checked_add_nonnegative(
            result.part_sum[result.part_of_item[item]],
            item_weight[item]
        );
    }
    return result;
}

template<class Weight>
PartitionResult<Weight> karmarkar_karp_partition(
    const std::vector<Weight>& item_weight
){
    return largest_differencing_partition(item_weight);
}

}  // namespace approximate::packing

#endif  // CPPLIB_SRC_APPROXIMATE_PACKING_NUMBER_PARTITIONING_HPP_INCLUDED
