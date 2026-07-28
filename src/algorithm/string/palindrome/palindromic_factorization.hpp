#ifndef CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_PALINDROMIC_FACTORIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_PALINDROMIC_FACTORIZATION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

struct PalindromicFactorizationResult{
    int factor_count = 0;
    std::vector<std::pair<int, int>> factors;
};

inline PalindromicFactorizationResult palindromic_factorization(
    const std::string& text
){
    struct Node{
        std::array<int, 256> next;
        int link = 0;
        int length = 0;
        int difference = 0;
        int series_link = 0;

        Node(){
            next.fill(-1);
        }
    };
    std::vector<Node> nodes(2);
    nodes[0].length = -1;
    nodes[0].link = 0;
    nodes[1].length = 0;
    nodes[1].link = 0;
    int last = 1;
    const int size = static_cast<int>(text.size());
    const int infinity = std::numeric_limits<int>::max() / 4;
    std::vector<int> minimum(static_cast<std::size_t>(size + 1), infinity);
    std::vector<int> previous(static_cast<std::size_t>(size + 1), -1);
    std::vector<int> series_value(2, infinity);
    std::vector<int> series_length(2, 0);
    minimum[0] = 0;
    for(int position = 0; position < size; ++position){
        const unsigned char symbol =
            static_cast<unsigned char>(text[static_cast<std::size_t>(position)]);
        int cursor = last;
        while(position - 1 - nodes[static_cast<std::size_t>(cursor)].length < 0
            || text[static_cast<std::size_t>(
                position - 1
                - nodes[static_cast<std::size_t>(cursor)].length
            )] != static_cast<char>(symbol)){
            cursor = nodes[static_cast<std::size_t>(cursor)].link;
        }
        int next = nodes[static_cast<std::size_t>(cursor)]
            .next[static_cast<std::size_t>(symbol)];
        if(next == -1){
            next = static_cast<int>(nodes.size());
            nodes.emplace_back();
            series_value.push_back(infinity);
            series_length.push_back(0);
            nodes[static_cast<std::size_t>(next)].length =
                nodes[static_cast<std::size_t>(cursor)].length + 2;
            nodes[static_cast<std::size_t>(cursor)]
                .next[static_cast<std::size_t>(symbol)] = next;
            if(nodes[static_cast<std::size_t>(next)].length == 1){
                nodes[static_cast<std::size_t>(next)].link = 1;
            }else{
                int suffix = nodes[static_cast<std::size_t>(cursor)].link;
                while(position - 1
                        - nodes[static_cast<std::size_t>(suffix)].length < 0
                    || text[static_cast<std::size_t>(
                        position - 1
                        - nodes[static_cast<std::size_t>(suffix)].length
                    )] != static_cast<char>(symbol)){
                    suffix = nodes[static_cast<std::size_t>(suffix)].link;
                }
                nodes[static_cast<std::size_t>(next)].link =
                    nodes[static_cast<std::size_t>(suffix)]
                        .next[static_cast<std::size_t>(symbol)];
            }
            Node& created = nodes[static_cast<std::size_t>(next)];
            created.difference = created.length
                - nodes[static_cast<std::size_t>(created.link)].length;
            created.series_link =
                created.difference
                    == nodes[static_cast<std::size_t>(created.link)].difference
                ? nodes[static_cast<std::size_t>(created.link)].series_link
                : created.link;
        }
        last = next;
        const int prefix_length = position + 1;
        for(int node = last; nodes[static_cast<std::size_t>(node)].length > 0;
            node = nodes[static_cast<std::size_t>(node)].series_link){
            const Node& current = nodes[static_cast<std::size_t>(node)];
            const int base_length =
                nodes[static_cast<std::size_t>(current.series_link)].length
                + current.difference;
            series_value[static_cast<std::size_t>(node)] =
                minimum[static_cast<std::size_t>(
                    prefix_length - base_length
                )];
            series_length[static_cast<std::size_t>(node)] = base_length;
            const int suffix = current.link;
            if(current.difference
                == nodes[static_cast<std::size_t>(suffix)].difference
                && series_value[static_cast<std::size_t>(suffix)]
                    < series_value[static_cast<std::size_t>(node)]){
                series_value[static_cast<std::size_t>(node)] =
                    series_value[static_cast<std::size_t>(suffix)];
                series_length[static_cast<std::size_t>(node)] =
                    series_length[static_cast<std::size_t>(suffix)];
            }
            const int candidate =
                series_value[static_cast<std::size_t>(node)] + 1;
            if(candidate < minimum[static_cast<std::size_t>(prefix_length)]){
                minimum[static_cast<std::size_t>(prefix_length)] = candidate;
                previous[static_cast<std::size_t>(prefix_length)] =
                    prefix_length
                    - series_length[static_cast<std::size_t>(node)];
            }
        }
    }
    PalindromicFactorizationResult result;
    result.factor_count = minimum[static_cast<std::size_t>(size)];
    for(int right = size; right > 0;){
        const int left = previous[static_cast<std::size_t>(right)];
        result.factors.push_back({left, right});
        right = left;
    }
    std::reverse(result.factors.begin(), result.factors.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_PALINDROMIC_FACTORIZATION_HPP_INCLUDED
