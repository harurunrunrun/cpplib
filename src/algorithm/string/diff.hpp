#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

enum class DiffTag{
    Equal,
    Insert,
    Erase,
};

template<class T>
struct DiffOp{
    DiffTag tag;
    T value;
};

namespace diff_internal{

template<class Sequence>
std::vector<int> lcs_prefix(
    const Sequence& a,
    int a_left,
    int a_right,
    const Sequence& b,
    int b_left,
    int b_right
){
    const int m = b_right - b_left;
    std::vector<int> previous(static_cast<std::size_t>(m + 1));
    std::vector<int> current(static_cast<std::size_t>(m + 1));
    for(int i = a_left; i < a_right; ++i){
        current[0] = 0;
        for(int j = 0; j < m; ++j){
            current[static_cast<std::size_t>(j + 1)] =
                a[static_cast<std::size_t>(i)] == b[static_cast<std::size_t>(b_left + j)]
                    ? previous[static_cast<std::size_t>(j)] + 1
                    : std::max(
                        previous[static_cast<std::size_t>(j + 1)],
                        current[static_cast<std::size_t>(j)]
                    );
        }
        previous.swap(current);
    }
    return previous;
}

template<class Sequence>
std::vector<int> lcs_suffix(
    const Sequence& a,
    int a_left,
    int a_right,
    const Sequence& b,
    int b_left,
    int b_right
){
    const int m = b_right - b_left;
    std::vector<int> next(static_cast<std::size_t>(m + 1));
    std::vector<int> current(static_cast<std::size_t>(m + 1));
    for(int i = a_right - 1; i >= a_left; --i){
        current[static_cast<std::size_t>(m)] = 0;
        for(int j = m - 1; j >= 0; --j){
            current[static_cast<std::size_t>(j)] =
                a[static_cast<std::size_t>(i)] == b[static_cast<std::size_t>(b_left + j)]
                    ? next[static_cast<std::size_t>(j + 1)] + 1
                    : std::max(
                        next[static_cast<std::size_t>(j)],
                        current[static_cast<std::size_t>(j + 1)]
                    );
        }
        next.swap(current);
    }
    return next;
}

template<class Sequence, class T>
void hirschberg(
    const Sequence& a,
    int a_left,
    int a_right,
    const Sequence& b,
    int b_left,
    int b_right,
    std::vector<DiffOp<T>>& result
){
    if(a_left == a_right){
        for(int j = b_left; j < b_right; ++j){
            result.push_back({DiffTag::Insert, b[static_cast<std::size_t>(j)]});
        }
        return;
    }
    if(b_left == b_right){
        for(int i = a_left; i < a_right; ++i){
            result.push_back({DiffTag::Erase, a[static_cast<std::size_t>(i)]});
        }
        return;
    }
    if(a_right - a_left == 1){
        int match = b_left;
        while(match < b_right &&
              a[static_cast<std::size_t>(a_left)] != b[static_cast<std::size_t>(match)]){
            ++match;
        }
        if(match == b_right){
            result.push_back({DiffTag::Erase, a[static_cast<std::size_t>(a_left)]});
            for(int j = b_left; j < b_right; ++j){
                result.push_back({DiffTag::Insert, b[static_cast<std::size_t>(j)]});
            }
        }else{
            for(int j = b_left; j < match; ++j){
                result.push_back({DiffTag::Insert, b[static_cast<std::size_t>(j)]});
            }
            result.push_back({DiffTag::Equal, a[static_cast<std::size_t>(a_left)]});
            for(int j = match + 1; j < b_right; ++j){
                result.push_back({DiffTag::Insert, b[static_cast<std::size_t>(j)]});
            }
        }
        return;
    }

    const int a_middle = a_left + (a_right - a_left) / 2;
    std::vector<int> left = lcs_prefix(a, a_left, a_middle, b, b_left, b_right);
    std::vector<int> right = lcs_suffix(a, a_middle, a_right, b, b_left, b_right);
    const int m = b_right - b_left;
    int split = 0;
    for(int j = 1; j <= m; ++j){
        if(left[static_cast<std::size_t>(split)] + right[static_cast<std::size_t>(split)] <
           left[static_cast<std::size_t>(j)] + right[static_cast<std::size_t>(j)]){
            split = j;
        }
    }
    std::vector<int>().swap(left);
    std::vector<int>().swap(right);
    hirschberg(a, a_left, a_middle, b, b_left, b_left + split, result);
    hirschberg(a, a_middle, a_right, b, b_left + split, b_right, result);
}

} // namespace diff_internal

template<class Sequence>
std::vector<DiffOp<typename Sequence::value_type>> diff_sequence(const Sequence& a, const Sequence& b){
    using T = typename Sequence::value_type;
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    const int maximum = n + m;
    const int offset = maximum + 1;
    std::vector<int> furthest(static_cast<std::size_t>(2 * maximum + 3), -1);
    furthest[static_cast<std::size_t>(offset + 1)] = 0;
    std::vector<std::vector<int>> trace;
    constexpr std::size_t trace_limit = 8U * 1024U * 1024U;
    std::size_t trace_entries = 0;
    int distance = 0;
    bool found = false;

    for(int d = 0; d <= maximum && !found; ++d){
        if(trace_entries + static_cast<std::size_t>(d + 1) > trace_limit){
            std::vector<DiffOp<T>> result;
            result.reserve(static_cast<std::size_t>(maximum));
            diff_internal::hirschberg(a, 0, n, b, 0, m, result);
            return result;
        }
        std::vector<int> row(static_cast<std::size_t>(d + 1));
        for(int diagonal = -d; diagonal <= d; diagonal += 2){
            int x;
            if(diagonal == -d ||
               (diagonal != d &&
                furthest[static_cast<std::size_t>(offset + diagonal - 1)] <
                furthest[static_cast<std::size_t>(offset + diagonal + 1)])){
                x = furthest[static_cast<std::size_t>(offset + diagonal + 1)];
            }else{
                x = furthest[static_cast<std::size_t>(offset + diagonal - 1)] + 1;
            }
            int y = x - diagonal;
            while(x < n && y < m &&
                  a[static_cast<std::size_t>(x)] == b[static_cast<std::size_t>(y)]){
                ++x;
                ++y;
            }
            furthest[static_cast<std::size_t>(offset + diagonal)] = x;
            row[static_cast<std::size_t>((diagonal + d) / 2)] = x;
            if(x == n && y == m){
                distance = d;
                found = true;
                break;
            }
        }
        trace_entries += row.size();
        trace.push_back(std::move(row));
    }

    std::vector<DiffOp<T>> result;
    result.reserve(static_cast<std::size_t>(maximum));
    int x = n;
    int y = m;
    for(int d = distance; d > 0; --d){
        const int diagonal = x - y;
        const auto& previous = trace[static_cast<std::size_t>(d - 1)];
        auto previous_x = [&](int k){
            return previous[static_cast<std::size_t>((k + d - 1) / 2)];
        };
        const int previous_diagonal =
            diagonal == -d ||
            (diagonal != d && previous_x(diagonal - 1) < previous_x(diagonal + 1))
                ? diagonal + 1
                : diagonal - 1;
        const int before_x = previous_x(previous_diagonal);
        const int before_y = before_x - previous_diagonal;
        while(x > before_x && y > before_y){
            result.push_back({DiffTag::Equal, a[static_cast<std::size_t>(x - 1)]});
            --x;
            --y;
        }
        if(x == before_x){
            result.push_back({DiffTag::Insert, b[static_cast<std::size_t>(y - 1)]});
            --y;
        }else{
            result.push_back({DiffTag::Erase, a[static_cast<std::size_t>(x - 1)]});
            --x;
        }
    }
    while(x > 0 && y > 0){
        result.push_back({DiffTag::Equal, a[static_cast<std::size_t>(x - 1)]});
        --x;
        --y;
    }
    std::reverse(result.begin(), result.end());
    return result;
}
