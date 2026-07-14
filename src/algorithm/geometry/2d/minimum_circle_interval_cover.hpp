#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "constants.hpp"

inline int minimum_circle_interval_cover(
    const std::vector<std::pair<long double, long double>>& intervals
){
    struct Arc{
        long double left;
        long double right;
    };

    const long double turn = 2.0L * std::acos(-1.0L);
    const auto normalize = [turn](long double angle){
        angle = std::fmod(angle, turn);
        if(angle < 0.0L) angle += turn;
        if(angle >= turn) angle = 0.0L;
        return angle;
    };

    std::vector<Arc> original;
    original.reserve(intervals.size());
    for(const auto& [start, end]: intervals){
        if(!std::isfinite(start) || !std::isfinite(end))[[unlikely]]{
            throw std::invalid_argument("circle interval endpoint must be finite");
        }
        if(std::fabs(end - start) >= turn - GEOMETRY_EPS) return 1;
        const long double left = normalize(start);
        long double length = normalize(end) - left;
        if(length < 0.0L) length += turn;
        if(length <= GEOMETRY_EPS) continue;
        original.push_back({left, left + length});
    }
    if(original.empty()) return -1;

    std::vector<Arc> arcs;
    arcs.reserve(original.size() * 2);
    for(const Arc& arc: original){
        arcs.push_back(arc);
        arcs.push_back({arc.left + turn, arc.right + turn});
    }
    std::sort(arcs.begin(), arcs.end(), [](const Arc& first, const Arc& second){
        if(first.left != second.left) return first.left < second.left;
        return first.right > second.right;
    });

    const int size = static_cast<int>(arcs.size());
    std::vector<long double> lefts(static_cast<std::size_t>(size));
    std::vector<int> prefix_best(static_cast<std::size_t>(size));
    int best = 0;
    for(int index = 0; index < size; ++index){
        lefts[static_cast<std::size_t>(index)] =
            arcs[static_cast<std::size_t>(index)].left;
        if(arcs[static_cast<std::size_t>(index)].right
            > arcs[static_cast<std::size_t>(best)].right){
            best = index;
        }
        prefix_best[static_cast<std::size_t>(index)] = best;
    }

    int levels = 1;
    while((1LL << levels) <= size) ++levels;
    std::vector<std::vector<int>> jump(
        static_cast<std::size_t>(levels),
        std::vector<int>(static_cast<std::size_t>(size))
    );
    for(int index = 0; index < size; ++index){
        const long double reach = arcs[static_cast<std::size_t>(index)].right
            + GEOMETRY_EPS;
        int last = static_cast<int>(
            std::upper_bound(lefts.begin(), lefts.end(), reach) - lefts.begin()
        ) - 1;
        int next = prefix_best[static_cast<std::size_t>(last)];
        if(arcs[static_cast<std::size_t>(next)].right
            <= arcs[static_cast<std::size_t>(index)].right + GEOMETRY_EPS){
            next = index;
        }
        jump[0][static_cast<std::size_t>(index)] = next;
    }
    for(int level = 1; level < levels; ++level){
        for(int index = 0; index < size; ++index){
            jump[static_cast<std::size_t>(level)]
                [static_cast<std::size_t>(index)] =
                jump[static_cast<std::size_t>(level - 1)]
                    [static_cast<std::size_t>(
                        jump[static_cast<std::size_t>(level - 1)]
                            [static_cast<std::size_t>(index)]
                    )];
        }
    }

    int answer = std::numeric_limits<int>::max();
    for(int start = 0; start < size; ++start){
        const Arc& first = arcs[static_cast<std::size_t>(start)];
        if(first.left >= turn - GEOMETRY_EPS) break;
        const long double target = first.left + turn;
        int current = start;
        int count = 1;
        for(int level = levels - 1; level >= 0; --level){
            const int next = jump[static_cast<std::size_t>(level)]
                [static_cast<std::size_t>(current)];
            if(next != current
                && arcs[static_cast<std::size_t>(next)].right
                    < target - GEOMETRY_EPS){
                current = next;
                count += 1 << level;
            }
        }
        if(arcs[static_cast<std::size_t>(current)].right
            >= target - GEOMETRY_EPS){
            answer = std::min(answer, count);
            continue;
        }
        const int next = jump[0][static_cast<std::size_t>(current)];
        if(next != current
            && arcs[static_cast<std::size_t>(next)].right
                >= target - GEOMETRY_EPS){
            answer = std::min(answer, count + 1);
        }
    }
    return answer == std::numeric_limits<int>::max() ? -1 : answer;
}
