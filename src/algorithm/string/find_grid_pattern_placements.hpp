#ifndef CPPLIB_SRC_ALGORITHM_STRING_FIND_GRID_PATTERN_PLACEMENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_FIND_GRID_PATTERN_PLACEMENTS_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

struct GridPatternPlacement{
    int row = -1;
    int column = -1;
    int direction = -1;
};

inline std::vector<GridPatternPlacement> find_grid_pattern_placements(
    const std::vector<std::string>& grid,
    const std::vector<std::string>& patterns
){
    if(grid.empty() || grid.front().empty())[[unlikely]]{
        throw std::invalid_argument("grid pattern search grid is empty");
    }
    if(grid.size() > static_cast<std::size_t>(
           std::numeric_limits<int>::max()
       ) || grid.front().size() > static_cast<std::size_t>(
           std::numeric_limits<int>::max()
       ))[[unlikely]]{
        throw std::length_error("grid pattern search grid is too large");
    }
    const int row_count = static_cast<int>(grid.size());
    const int column_count = static_cast<int>(grid.front().size());
    for(const auto& row: grid){
        if(static_cast<int>(row.size()) != column_count)[[unlikely]]{
            throw std::invalid_argument("grid pattern search grid is ragged");
        }
        for(const char character: row){
            if(character < 'A' || 'Z' < character)[[unlikely]]{
                throw std::invalid_argument("grid pattern search invalid grid character");
            }
        }
    }
    struct Node{
        std::array<int, 26> next{};
        int link = 0;
        int output = -1;
        std::vector<int> pattern_ids;

        Node(){
            next.fill(-1);
        }
    };
    std::vector<Node> nodes(1);
    if(patterns.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("grid pattern search pattern count is too large");
    }
    for(int pattern_id = 0; pattern_id < static_cast<int>(patterns.size()); ++pattern_id){
        const auto& pattern = patterns[static_cast<std::size_t>(pattern_id)];
        if(pattern.empty())[[unlikely]]{
            throw std::invalid_argument("grid pattern search contains empty pattern");
        }
        int state = 0;
        for(const char character: pattern){
            if(character < 'A' || 'Z' < character)[[unlikely]]{
                throw std::invalid_argument("grid pattern search invalid pattern character");
            }
            const std::size_t letter = static_cast<std::size_t>(character - 'A');
            int& to = nodes[static_cast<std::size_t>(state)].next[letter];
            if(to == -1){
                if(nodes.size() >= static_cast<std::size_t>(
                    std::numeric_limits<int>::max()
                ))[[unlikely]]{
                    throw std::length_error("grid pattern search dictionary too large");
                }
                const int created = static_cast<int>(nodes.size());
                to = created;
                nodes.emplace_back();
                state = created;
            }else{
                state = to;
            }
        }
        nodes[static_cast<std::size_t>(state)].pattern_ids.push_back(pattern_id);
    }

    std::queue<int> queue;
    for(std::size_t letter = 0; letter < 26; ++letter){
        int& to = nodes[0].next[letter];
        if(to == -1){
            to = 0;
        }else{
            queue.push(to);
        }
    }
    while(!queue.empty()){
        const int vertex = queue.front();
        queue.pop();
        Node& node = nodes[static_cast<std::size_t>(vertex)];
        const Node& failure = nodes[static_cast<std::size_t>(node.link)];
        node.output = !failure.pattern_ids.empty() ? node.link : failure.output;
        for(std::size_t letter = 0; letter < 26; ++letter){
            int& to = node.next[letter];
            if(to == -1){
                to = failure.next[letter];
            }else{
                nodes[static_cast<std::size_t>(to)].link = failure.next[letter];
                queue.push(to);
            }
        }
    }

    std::vector<GridPatternPlacement> answer(patterns.size());
    std::vector<int> active_parent(nodes.size(), -1);
    for(std::size_t state = 0; state < nodes.size(); ++state){
        if(!nodes[state].pattern_ids.empty()){
            active_parent[state] = static_cast<int>(state);
        }
    }
    const auto find_active = [&](int state) {
        int current = state;
        while(current != -1 &&
              nodes[static_cast<std::size_t>(current)].pattern_ids.empty()){
            current = active_parent[static_cast<std::size_t>(current)];
        }
        const int root = current;
        current = state;
        while(current != -1 && current != root){
            const int next = active_parent[static_cast<std::size_t>(current)];
            active_parent[static_cast<std::size_t>(current)] = root;
            current = next;
        }
        return root;
    };
    for(std::size_t state = 0; state < nodes.size(); ++state){
        if(nodes[state].pattern_ids.empty()){
            active_parent[state] = nodes[state].output;
        }
    }

    constexpr std::array<int, 8> DR = {-1, -1, 0, 1, 1, 1, 0, -1};
    constexpr std::array<int, 8> DC = {0, 1, 1, 1, 0, -1, -1, -1};
    const auto inside = [&](int row, int column){
        return 0 <= row && row < row_count &&
            0 <= column && column < column_count;
    };
    for(int direction = 0; direction < 8; ++direction){
        const int dr = DR[static_cast<std::size_t>(direction)];
        const int dc = DC[static_cast<std::size_t>(direction)];
        for(int start_row = 0; start_row < row_count; ++start_row){
            for(int start_column = 0; start_column < column_count;
                ++start_column){
                if(inside(start_row - dr, start_column - dc)) continue;
                int state = 0;
                for(int row = start_row, column = start_column;
                    inside(row, column); row += dr, column += dc){
                    state = nodes[static_cast<std::size_t>(state)].next[
                        static_cast<std::size_t>(
                            grid[static_cast<std::size_t>(row)]
                                [static_cast<std::size_t>(column)] - 'A'
                        )
                    ];
                    int output = !nodes[static_cast<std::size_t>(state)]
                        .pattern_ids.empty() ? state
                        : nodes[static_cast<std::size_t>(state)].output;
                    while((output = find_active(output)) != -1){
                        auto& ids = nodes[static_cast<std::size_t>(output)]
                            .pattern_ids;
                        for(const int pattern_id: ids){
                            const int offset = static_cast<int>(
                                patterns[static_cast<std::size_t>(pattern_id)].size()
                            ) - 1;
                            answer[static_cast<std::size_t>(pattern_id)] = {
                                row - offset * dr,
                                column - offset * dc,
                                direction,
                            };
                        }
                        ids.clear();
                        active_parent[static_cast<std::size_t>(output)] =
                            nodes[static_cast<std::size_t>(output)].output;
                    }
                }
            }
        }
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_FIND_GRID_PATTERN_PLACEMENTS_HPP_INCLUDED
