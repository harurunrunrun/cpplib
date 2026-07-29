#ifndef CPPLIB_SRC_APPROXIMATE_PACKING_ADVANCED_BIN_PACKING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_PACKING_ADVANCED_BIN_PACKING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "bin_packing.hpp"

namespace approximate::packing {

template<class Size>
[[nodiscard]] BinPackingResult<Size> next_fit_decreasing_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity
) {
    return next_fit_bin_packing(item_size, capacity, true);
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> first_fit_decreasing_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity
) {
    return first_fit_bin_packing(item_size, capacity, true);
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> best_fit_decreasing_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity
) {
    return best_fit_bin_packing(item_size, capacity, true);
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> worst_fit_decreasing_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity
) {
    return worst_fit_bin_packing(item_size, capacity, true);
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> almost_worst_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    bool decreasing = false
) {
    internal::validate_items(item_size, capacity);
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    std::set<std::pair<Size, std::size_t>> available;
    for(const std::size_t item :
        internal::packing_order(item_size, decreasing)){
        const auto first_fitting =
            available.lower_bound({item_size[item], 0});
        std::size_t bin = result.remaining_capacity.size();
        if(first_fitting != available.end()){
            auto selected = std::prev(available.end());
            if(selected != first_fitting){
                --selected;
            }
            bin = selected->second;
            available.erase(selected);
        }else{
            result.remaining_capacity.push_back(capacity);
        }
        result.bin_of_item[item] = bin;
        result.remaining_capacity[bin] =
            result.remaining_capacity[bin] - item_size[item];
        available.emplace(result.remaining_capacity[bin], bin);
    }
    return result;
}

namespace advanced_bin_packing_internal {

template<class Size>
std::size_t harmonic_class(
    const Size& size,
    const Size& capacity,
    std::size_t class_count
) {
    const long double ratio = static_cast<long double>(size)
        / static_cast<long double>(capacity);
    for(std::size_t group = 1; group < class_count; ++group){
        if(ratio > 1.0L / static_cast<long double>(group + 1)){
            return group;
        }
    }
    return class_count;
}

template<class Size>
BinPackingResult<Size> harmonic_impl(
    const std::vector<Size>& item_size,
    const Size& capacity,
    std::size_t class_count,
    bool first_fit_small
) {
    internal::validate_items(item_size, capacity);
    if(class_count < 2){
        throw std::invalid_argument(
            "harmonic packing requires at least two classes"
        );
    }
    std::vector<std::vector<std::size_t>> classes(class_count + 1);
    for(std::size_t item = 0; item < item_size.size(); ++item){
        classes[harmonic_class(
            item_size[item], capacity, class_count
        )].push_back(item);
    }
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    for(std::size_t group = 1; group < class_count; ++group){
        std::size_t in_current = group;
        std::size_t bin = 0;
        for(const std::size_t item : classes[group]){
            if(in_current == group){
                bin = result.remaining_capacity.size();
                result.remaining_capacity.push_back(capacity);
                in_current = 0;
            }
            result.bin_of_item[item] = bin;
            result.remaining_capacity[bin] =
                result.remaining_capacity[bin] - item_size[item];
            ++in_current;
        }
    }
    std::vector<std::size_t> small_bins;
    std::size_t current = result.remaining_capacity.size();
    for(const std::size_t item : classes[class_count]){
        std::size_t bin = result.remaining_capacity.size();
        if(first_fit_small){
            for(const std::size_t candidate : small_bins){
                if(!(result.remaining_capacity[candidate] < item_size[item])){
                    bin = candidate;
                    break;
                }
            }
        }else if(current < result.remaining_capacity.size()
                 && !(result.remaining_capacity[current] < item_size[item])){
            bin = current;
        }
        if(bin == result.remaining_capacity.size()){
            bin = result.remaining_capacity.size();
            result.remaining_capacity.push_back(capacity);
            small_bins.push_back(bin);
            current = bin;
        }
        result.bin_of_item[item] = bin;
        result.remaining_capacity[bin] =
            result.remaining_capacity[bin] - item_size[item];
    }
    return result;
}

template<class Size>
std::size_t first_fitting_bin(
    const BinPackingResult<Size>& result,
    const Size& size,
    const std::vector<unsigned char>* allowed = nullptr
) {
    for(std::size_t bin = 0; bin < result.remaining_capacity.size(); ++bin){
        if(allowed != nullptr && (*allowed)[bin] == 0) continue;
        if(!(result.remaining_capacity[bin] < size)) return bin;
    }
    return result.remaining_capacity.size();
}

template<class Size>
void place_item(
    BinPackingResult<Size>& result,
    const std::vector<Size>& item_size,
    std::size_t item,
    std::size_t bin,
    const Size& capacity
) {
    if(bin == result.remaining_capacity.size()){
        result.remaining_capacity.push_back(capacity);
    }
    result.bin_of_item[item] = bin;
    result.remaining_capacity[bin] =
        result.remaining_capacity[bin] - item_size[item];
}

}  // namespace advanced_bin_packing_internal

template<class Size>
[[nodiscard]] BinPackingResult<Size> harmonic_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    std::size_t class_count = 10
) {
    return advanced_bin_packing_internal::harmonic_impl(
        item_size, capacity, class_count, false
    );
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> harmonic_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    std::size_t class_count = 10
) {
    return harmonic_bin_packing(item_size, capacity, class_count);
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> harmonic_first_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity,
    std::size_t class_count = 10
) {
    return advanced_bin_packing_internal::harmonic_impl(
        item_size, capacity, class_count, true
    );
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> refined_first_fit_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity
) {
    using namespace advanced_bin_packing_internal;
    internal::validate_items(item_size, capacity);
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    std::vector<unsigned char> reserved_medium_bin;
    std::size_t medium_count = 0;
    for(std::size_t item = 0; item < item_size.size(); ++item){
        const long double ratio = static_cast<long double>(item_size[item])
            / static_cast<long double>(capacity);
        std::size_t bin = result.remaining_capacity.size();
        if(ratio > 0.5L){
            for(std::size_t candidate = 0;
                candidate < result.remaining_capacity.size(); ++candidate){
                if(reserved_medium_bin[candidate] != 0
                   && !(result.remaining_capacity[candidate]
                        < item_size[item])){
                    bin = candidate;
                    break;
                }
            }
        }else if(ratio > 0.4L){
            ++medium_count;
            if(medium_count % 6U != 0U){
                for(std::size_t candidate = 0;
                    candidate < result.remaining_capacity.size(); ++candidate){
                    if(reserved_medium_bin[candidate] == 0
                       && !(result.remaining_capacity[candidate]
                            < item_size[item])){
                        bin = candidate;
                        break;
                    }
                }
            }
        }else{
            bin = first_fitting_bin(result, item_size[item]);
        }
        const bool make_reserved =
            ratio > 0.4L && ratio <= 0.5L && medium_count % 6U == 0U
            && bin == result.remaining_capacity.size();
        place_item(result, item_size, item, bin, capacity);
        if(reserved_medium_bin.size() < result.remaining_capacity.size()){
            reserved_medium_bin.push_back(
                static_cast<unsigned char>(make_reserved)
            );
        }else if(ratio > 0.5L){
            reserved_medium_bin[bin] = 0;
        }
    }
    return result;
}

template<class Size>
[[nodiscard]] BinPackingResult<Size>
modified_first_fit_decreasing_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity
) {
    using namespace advanced_bin_packing_internal;
    internal::validate_items(item_size, capacity);
    std::vector<std::size_t> order =
        internal::packing_order(item_size, true);
    BinPackingResult<Size> result;
    result.bin_of_item.resize(item_size.size());
    std::vector<std::size_t> large_bins;
    for(const std::size_t item : order){
        const long double ratio = static_cast<long double>(item_size[item])
            / static_cast<long double>(capacity);
        std::size_t bin = result.remaining_capacity.size();
        if(ratio > 0.5L){
            place_item(result, item_size, item, bin, capacity);
            large_bins.push_back(bin);
            continue;
        }
        if(ratio > 1.0L / 6.0L){
            Size best_remaining{};
            bool found = false;
            for(const std::size_t candidate : large_bins){
                if(result.remaining_capacity[candidate] < item_size[item]){
                    continue;
                }
                const Size remaining =
                    result.remaining_capacity[candidate] - item_size[item];
                if(!found || remaining < best_remaining){
                    found = true;
                    best_remaining = remaining;
                    bin = candidate;
                }
            }
        }
        if(bin == result.remaining_capacity.size()){
            bin = first_fitting_bin(result, item_size[item]);
        }
        place_item(result, item_size, item, bin, capacity);
    }
    return result;
}

template<class Size>
[[nodiscard]] BinPackingResult<Size> mffd_bin_packing(
    const std::vector<Size>& item_size,
    const Size& capacity
) {
    return modified_first_fit_decreasing_bin_packing(item_size, capacity);
}

}  // namespace approximate::packing

#endif  // CPPLIB_SRC_APPROXIMATE_PACKING_ADVANCED_BIN_PACKING_HPP_INCLUDED
