#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_FRACTIONAL_CASCADING_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_FRACTIONAL_CASCADING_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
class FractionalCascading{
    struct Entry{ T value; int current, next; };
    std::vector<std::vector<T>> catalogs_;
    std::vector<std::vector<Entry>> merged_;

public:
    FractionalCascading() = default;
    explicit FractionalCascading(std::vector<std::vector<T>> catalogs): catalogs_(std::move(catalogs)){
        for(auto& catalog: catalogs_) if(!std::is_sorted(catalog.begin(), catalog.end()))
            throw std::invalid_argument("FractionalCascading: catalog is not sorted");
        merged_.resize(catalogs_.size());
        for(int layer = static_cast<int>(catalogs_.size()) - 1; layer >= 0; --layer){
            std::vector<T> values = catalogs_[layer];
            if(layer + 1 < static_cast<int>(catalogs_.size()))
                for(std::size_t i = 1; i < merged_[layer + 1].size(); i += 2) values.push_back(merged_[layer + 1][i].value);
            std::sort(values.begin(), values.end());
            values.erase(std::unique(values.begin(), values.end()), values.end());
            auto& output = merged_[layer];
            output.reserve(values.size());
            for(const T& value: values){
                output.push_back({value,
                    static_cast<int>(std::lower_bound(catalogs_[layer].begin(), catalogs_[layer].end(), value)
                                     - catalogs_[layer].begin()),
                    layer + 1 == static_cast<int>(catalogs_.size()) ? 0 :
                    static_cast<int>(std::lower_bound(merged_[layer + 1].begin(), merged_[layer + 1].end(), value,
                        [](const Entry& entry, const T& target){ return entry.value < target; }) - merged_[layer + 1].begin())});
            }
        }
    }
    int catalog_count() const noexcept { return static_cast<int>(catalogs_.size()); }
    std::vector<int> lower_bounds(const T& value) const {
        std::vector<int> answer(catalogs_.size());
        if(catalogs_.empty()) return answer;
        int position = static_cast<int>(std::lower_bound(merged_[0].begin(), merged_[0].end(), value,
            [](const Entry& entry, const T& target){ return entry.value < target; }) - merged_[0].begin());
        for(int layer = 0; layer < static_cast<int>(catalogs_.size()); ++layer){
            if(position == static_cast<int>(merged_[layer].size())){
                answer[layer] = static_cast<int>(catalogs_[layer].size());
                if(layer + 1 < static_cast<int>(catalogs_.size())) position = static_cast<int>(merged_[layer + 1].size());
            }else{
                answer[layer] = merged_[layer][position].current;
                position = merged_[layer][position].next;
            }
            if(layer + 1 < static_cast<int>(catalogs_.size())){
                while(position > 0 && !(merged_[layer + 1][position - 1].value < value)) --position;
            }
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_FRACTIONAL_CASCADING_HPP_INCLUDED
