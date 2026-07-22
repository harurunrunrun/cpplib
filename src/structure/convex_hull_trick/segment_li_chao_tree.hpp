#ifndef CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_SEGMENT_LI_CHAO_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_SEGMENT_LI_CHAO_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

class SegmentLiChaoTree{
public:
    struct Line{
        long long slope;
        long long intercept;

        __int128_t eval128(long long x) const{
            return static_cast<__int128_t>(slope) * x + intercept;
        }

        long long eval(long long x) const{
            const __int128_t value = eval128(x);
            if(value < static_cast<__int128_t>(std::numeric_limits<long long>::min()) ||
               static_cast<__int128_t>(std::numeric_limits<long long>::max()) < value)[[unlikely]]{
                throw std::overflow_error("SegmentLiChaoTree::Line::eval overflow");
            }
            return static_cast<long long>(value);
        }
    };

private:
    std::vector<long long> xs_;
    std::vector<std::optional<Line>> lines_;
    bool has_line_ = false;

    static bool less_at(const Line& left, const Line& right, long long x){
        return left.eval128(x) < right.eval128(x);
    }

    void add_line_impl(int node, int left, int right, Line line){
        auto& current = lines_[static_cast<std::size_t>(node)];
        if(!current){
            current = line;
            return;
        }
        const int middle = left + (right - left) / 2;
        const bool left_better = less_at(line, *current, xs_[static_cast<std::size_t>(left)]);
        const bool middle_better = less_at(line, *current, xs_[static_cast<std::size_t>(middle)]);
        if(middle_better) std::swap(line, *current);
        if(right - left == 1) return;
        if(left_better != middle_better){
            add_line_impl(node * 2, left, middle, line);
        }else{
            add_line_impl(node * 2 + 1, middle, right, line);
        }
    }

    void add_segment_impl(
        int node,
        int left,
        int right,
        int query_left,
        int query_right,
        const Line& line
    ){
        if(right <= query_left || query_right <= left) return;
        if(query_left <= left && right <= query_right){
            add_line_impl(node, left, right, line);
            return;
        }
        const int middle = left + (right - left) / 2;
        add_segment_impl(node * 2, left, middle, query_left, query_right, line);
        add_segment_impl(node * 2 + 1, middle, right, query_left, query_right, line);
    }

    int coordinate_index(long long x) const{
        const auto iterator = std::lower_bound(xs_.begin(), xs_.end(), x);
        if(iterator == xs_.end() || *iterator != x)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: unregistered coordinate (SegmentLiChaoTree)."
            );
        }
        return static_cast<int>(iterator - xs_.begin());
    }

public:
    explicit SegmentLiChaoTree(std::vector<long long> query_coordinates):
        xs_(std::move(query_coordinates)){
        std::sort(xs_.begin(), xs_.end());
        xs_.erase(std::unique(xs_.begin(), xs_.end()), xs_.end());
        lines_.resize(xs_.empty() ? 1 : xs_.size() * 4);
    }

    int size() const noexcept{ return static_cast<int>(xs_.size()); }
    bool empty() const noexcept{ return !has_line_; }
    const std::vector<long long>& coordinates() const noexcept{ return xs_; }

    void add_line(long long slope, long long intercept){
        add_line(Line{slope, intercept});
    }

    void add_line(Line line){
        if(xs_.empty()) return;
        add_line_impl(1, 0, size(), line);
        has_line_ = true;
    }

    void add_segment(
        long long left,
        long long right,
        long long slope,
        long long intercept
    ){
        add_segment(left, right, Line{slope, intercept});
    }

    void add_segment(long long left, long long right, Line line){
        if(right < left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (SegmentLiChaoTree::add_segment)."
            );
        }
        if(left == right || xs_.empty()) return;
        const int query_left = static_cast<int>(
            std::lower_bound(xs_.begin(), xs_.end(), left) - xs_.begin()
        );
        const int query_right = static_cast<int>(
            std::lower_bound(xs_.begin(), xs_.end(), right) - xs_.begin()
        );
        if(query_left == query_right) return;
        add_segment_impl(1, 0, size(), query_left, query_right, line);
        has_line_ = true;
    }

    std::optional<long long> query(long long x) const{
        int index = coordinate_index(x);
        int node = 1;
        int left = 0;
        int right = size();
        std::optional<__int128_t> answer;
        while(true){
            const auto& line = lines_[static_cast<std::size_t>(node)];
            if(line){
                const __int128_t value = line->eval128(x);
                if(!answer || value < *answer) answer = value;
            }
            if(right - left == 1) break;
            const int middle = left + (right - left) / 2;
            if(index < middle){
                node *= 2;
                right = middle;
            }else{
                node = node * 2 + 1;
                left = middle;
            }
        }
        if(!answer) return std::nullopt;
        if(*answer < static_cast<__int128_t>(std::numeric_limits<long long>::min()) ||
           static_cast<__int128_t>(std::numeric_limits<long long>::max()) < *answer)[[unlikely]]{
            throw std::overflow_error("SegmentLiChaoTree::query overflow");
        }
        return static_cast<long long>(*answer);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_SEGMENT_LI_CHAO_TREE_HPP_INCLUDED
