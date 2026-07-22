#ifndef CPPLIB_SRC_APPROXIMATE_PACKING_BIN_PACKING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_PACKING_BIN_PACKING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::packing {

template<class Size>
struct BinPackingResult{
    std::vector<std::size_t> bin_of_item;
    std::vector<Size> remaining_capacity;

    [[nodiscard]] std::size_t bin_count() const noexcept{
        return remaining_capacity.size();
    }
};

namespace internal {

template<class Size>
[[nodiscard]] bool is_finite_size(const Size& value){
    if constexpr(std::is_floating_point_v<Size>) return std::isfinite(value);
    return true;
}

template<class Size>
void validate_items(const std::vector<Size>& item_size, const Size& capacity){
    if(!is_finite_size(capacity) || !(Size{} < capacity))[[unlikely]]{
        throw std::invalid_argument("bin capacity must be positive");
    }
    for(const Size& size: item_size){
        if(!is_finite_size(size) || size < Size{} || capacity < size)[[unlikely]]{
            throw std::invalid_argument("item does not fit an empty bin");
        }
    }
}

template<class Size>
std::vector<std::size_t> packing_order(
    const std::vector<Size>& item_size,
    bool decreasing
){
    std::vector<std::size_t> order(item_size.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    if(decreasing){
        std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
            return item_size[second] < item_size[first];
        });
    }
    return order;
}

template<class Size>
class FirstFitTree{
public:
    explicit FirstFitTree(std::size_t size): base_(1){
        while(base_ < size) base_ <<= 1U;
        maximum_.assign(base_ * 2U, Size{});
    }

    void set(std::size_t index, const Size& value){
        index += base_;
        maximum_[index] = value;
        while(index > 1){
            index >>= 1U;
            maximum_[index] = maximum_[index * 2U] < maximum_[index * 2U + 1U]
                ? maximum_[index * 2U + 1U]
                : maximum_[index * 2U];
        }
    }

    [[nodiscard]] std::size_t first_at_least(const Size& value) const{
        if(maximum_[1] < value) return base_;
        std::size_t node = 1;
        while(node < base_){
            node = !(maximum_[node * 2U] < value)
                ? node * 2U
                : node * 2U + 1U;
        }
        return node - base_;
    }

private:
    std::size_t base_;
    std::vector<Size> maximum_;
};

}  // namespace internal

template<class Size>
BinPackingResult<Size> next_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    bool decreasing = false
){
    internal::validate_items(item_size, capacity);
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    for(const std::size_t item: internal::packing_order(item_size, decreasing)){
        if(result.remaining_capacity.empty()
            || result.remaining_capacity.back() < item_size[item]){
            result.remaining_capacity.push_back(capacity);
        }
        const std::size_t bin = result.remaining_capacity.size() - 1;
        result.bin_of_item[item] = bin;
        result.remaining_capacity[bin] =
            result.remaining_capacity[bin] - item_size[item];
    }
    return result;
}

template<class Size>
BinPackingResult<Size> first_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    bool decreasing = false
){
    internal::validate_items(item_size, capacity);
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    internal::FirstFitTree<Size> tree(item_size.size());
    for(const std::size_t item: internal::packing_order(item_size, decreasing)){
        std::size_t bin = tree.first_at_least(item_size[item]);
        if(bin >= result.remaining_capacity.size()){
            bin = result.remaining_capacity.size();
            result.remaining_capacity.push_back(capacity);
        }
        result.bin_of_item[item] = bin;
        result.remaining_capacity[bin] =
            result.remaining_capacity[bin] - item_size[item];
        tree.set(bin, result.remaining_capacity[bin]);
    }
    return result;
}

template<class Size>
BinPackingResult<Size> best_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    bool decreasing = false
){
    internal::validate_items(item_size, capacity);
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    std::set<std::pair<Size, std::size_t>> available;
    for(const std::size_t item: internal::packing_order(item_size, decreasing)){
        auto iterator = available.lower_bound({item_size[item], 0});
        std::size_t bin;
        if(iterator == available.end()){
            bin = result.remaining_capacity.size();
            result.remaining_capacity.push_back(capacity);
        }else{
            bin = iterator->second;
            available.erase(iterator);
        }
        result.bin_of_item[item] = bin;
        result.remaining_capacity[bin] =
            result.remaining_capacity[bin] - item_size[item];
        available.emplace(result.remaining_capacity[bin], bin);
    }
    return result;
}

template<class Size>
BinPackingResult<Size> worst_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    bool decreasing = false
){
    internal::validate_items(item_size, capacity);
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    std::set<std::pair<Size, std::size_t>> available;
    for(const std::size_t item: internal::packing_order(item_size, decreasing)){
        std::size_t bin;
        if(available.empty() || available.rbegin()->first < item_size[item]){
            bin = result.remaining_capacity.size();
            result.remaining_capacity.push_back(capacity);
        }else{
            auto iterator = std::prev(available.end());
            bin = iterator->second;
            available.erase(iterator);
        }
        result.bin_of_item[item] = bin;
        result.remaining_capacity[bin] =
            result.remaining_capacity[bin] - item_size[item];
        available.emplace(result.remaining_capacity[bin], bin);
    }
    return result;
}

}  // namespace approximate::packing

#endif  // CPPLIB_SRC_APPROXIMATE_PACKING_BIN_PACKING_HPP_INCLUDED
