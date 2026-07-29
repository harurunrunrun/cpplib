// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/approximate/search/multi_agent_pathfinding.hpp"
#include "../../src/approximate/search/real_time_heuristic_search.hpp"

namespace{

int grid_distance(
    const MultiAgentGridMap& map,
    GridLocation start,
    GridLocation goal
){
    std::vector<int> distance(static_cast<std::size_t>(map.size()), -1);
    std::queue<GridLocation> queue;
    distance[static_cast<std::size_t>(map.index(start))] = 0;
    queue.push(start);
    while(!queue.empty()){
        const GridLocation current = queue.front();
        queue.pop();
        for(const GridLocation next : map.neighbors(current)){
            int& value = distance[static_cast<std::size_t>(map.index(next))];
            if(value >= 0) continue;
            value = distance[static_cast<std::size_t>(map.index(current))] + 1;
            queue.push(next);
        }
    }
    return distance[static_cast<std::size_t>(map.index(goal))];
}

void verify_grid_path(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& path,
    GridLocation start,
    GridLocation goal
){
    assert(!path.empty() && path.front() == start && path.back() == goal);
    for(std::size_t i = 0; i < path.size(); ++i){
        assert(map.passable(path[i]));
        if(i != 0){
            const int difference =
                std::abs(path[i].row - path[i - 1].row)
              + std::abs(path[i].column - path[i - 1].column);
            assert(difference <= 1);
        }
    }
}

void verify_multi_agent(
    const MultiAgentGridMap& map,
    const std::vector<std::vector<GridLocation>>& paths,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals
){
    assert(paths.size() == starts.size());
    int makespan = 0;
    for(std::size_t agent = 0; agent < paths.size(); ++agent){
        verify_grid_path(map, paths[agent], starts[agent], goals[agent]);
        makespan = std::max(
            makespan, static_cast<int>(paths[agent].size()) - 1
        );
    }
    const auto at = [](const std::vector<GridLocation>& path, int time){
        return path[std::min<std::size_t>(
            static_cast<std::size_t>(time), path.size() - 1
        )];
    };
    for(int time = 0; time <= makespan; ++time){
        for(std::size_t first = 0; first < paths.size(); ++first){
            for(std::size_t second = first + 1;
                second < paths.size();
                ++second){
                assert(at(paths[first], time) != at(paths[second], time));
                if(time != makespan){
                    assert(!(at(paths[first], time) == at(paths[second], time + 1)
                        && at(paths[second], time) == at(paths[first], time + 1)));
                }
            }
        }
    }
}

void fixed_cases(){
    const MultiAgentGridMap line(1, 3);
    ReservationTable reservations;
    reservations.reserve_vertex(GridLocation{0, 1}, 1);
    const auto waited = sipp_search(
        line, GridLocation{0, 0}, GridLocation{0, 2},
        reservations, 0, 6
    );
    assert(waited && waited.cost == 3);
    assert(waited.path[1].position == GridLocation(0, 0));
    reservations.reserve_edge(
        GridLocation{0, 2}, GridLocation{0, 1}, 2
    );
    const auto edge_waited = safe_interval_path_planning(
        line, GridLocation{0, 0}, GridLocation{0, 2},
        reservations, 0, 8
    );
    assert(edge_waited && edge_waited.cost >= 4);
    const auto intervals = reservations.safe_intervals(
        GridLocation{0, 1}, 0, 3
    );
    assert((intervals == std::vector<std::pair<int, int>>{{0, 0}, {2, 3}}));
    assert(reservations.reserved_vertex_count() == 1);
    assert(reservations.reserved_edge_count() == 1);
    assert(reservations.maximum_time() == 3);

    const MultiAgentGridMap square(2, 2);
    const std::vector<GridLocation> starts{{0, 0}, {0, 1}};
    const std::vector<GridLocation> goals{{0, 1}, {0, 0}};
    const auto cbs = bounded_conflict_based_search(
        square, starts, goals, 1000, 12
    );
    assert(cbs);
    verify_multi_agent(square, cbs.paths, starts, goals);
    const auto enhanced = enhanced_cbs_search(
        square, starts, goals, 1000, 12
    );
    assert(enhanced && enhanced.sum_of_costs == cbs.sum_of_costs);
    const auto enhanced_long = enhanced_conflict_based_search(
        square, starts, goals, 1000, 12
    );
    assert(enhanced_long && enhanced_long.sum_of_costs == cbs.sum_of_costs);
    const auto ecbs = ecbs_search(square, starts, goals, 1.5L, 1000, 12);
    assert(ecbs && ecbs.sum_of_costs * 2 <= cbs.sum_of_costs * 3);
    const auto cutoff = bounded_conflict_based_search(
        square, starts, goals, 1, 12
    );
    assert(!cutoff && cutoff.cutoff);

    const MultiAgentGridMap lanes(2, 3);
    const std::vector<GridLocation> lane_starts{{0, 0}, {1, 0}};
    const std::vector<GridLocation> lane_goals{{0, 2}, {1, 2}};
    const auto cooperative = cooperative_a_star_search(
        lanes, lane_starts, lane_goals, 10
    );
    assert(cooperative);
    verify_multi_agent(lanes, cooperative.paths, lane_starts, lane_goals);
    const auto whca = whca_star_search(
        lanes, lane_starts, lane_goals, 3, 10
    );
    assert(whca);
    verify_multi_agent(lanes, whca.paths, lane_starts, lane_goals);
    const auto whca_long =
        windowed_hierarchical_cooperative_a_star_search(
            lanes, lane_starts, lane_goals, 3, 10
        );
    assert(whca_long && whca_long.sum_of_costs == whca.sum_of_costs);
}

} // namespace

int main(){
    fixed_cases();
    int rows;
    int columns;
    if(!(std::cin >> rows >> columns)) return 0;
    std::vector<std::string> cells(static_cast<std::size_t>(rows));
    for(std::string& row : cells) std::cin >> row;
    std::vector<GridLocation> blocked;
    for(int row = 0; row < rows; ++row){
        for(int column = 0; column < columns; ++column){
            if(cells[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)] == '#'){
                blocked.push_back(GridLocation{row, column});
            }
        }
    }
    const MultiAgentGridMap map(rows, columns, blocked);
    int agent_count;
    std::cin >> agent_count;
    std::vector<GridLocation> starts(static_cast<std::size_t>(agent_count));
    std::vector<GridLocation> goals(static_cast<std::size_t>(agent_count));
    for(int agent = 0; agent < agent_count; ++agent){
        std::cin >> starts[static_cast<std::size_t>(agent)].row
                 >> starts[static_cast<std::size_t>(agent)].column
                 >> goals[static_cast<std::size_t>(agent)].row
                 >> goals[static_cast<std::size_t>(agent)].column;
    }

    const int expected_single = grid_distance(map, starts[0], goals[0]);
    ReservationTable empty;
    const auto sipp = sipp_search(
        map, starts[0], goals[0], empty, 0, 40
    );
    const auto hpa = hpa_star_search(map, starts[0], goals[0], 2);
    assert(sipp && hpa);
    assert(sipp.cost == expected_single);
    assert(hpa.cost == expected_single);
    verify_grid_path(
        map,
        [&]{
            std::vector<GridLocation> result;
            for(const auto value : hpa.path) result.push_back(value.position);
            return result;
        }(),
        starts[0], goals[0]
    );

    const auto neighbors = [&](GridLocation position){
        std::vector<std::pair<GridLocation, int>> result;
        for(const GridLocation next : map.neighbors(position)){
            result.emplace_back(next, 1);
        }
        return result;
    };
    const auto goal = [&](GridLocation position){
        return position == goals[0];
    };
    const auto heuristic = [&](GridLocation position){
        return std::abs(position.row - goals[0].row)
             + std::abs(position.column - goals[0].column);
    };
    const auto lrta = lrta_star_search(
        starts[0], goal, neighbors, heuristic, 2000, GridLocationHash{}
    );
    const auto rta = rta_star_search(
        starts[0], goal, neighbors, heuristic, 2000, GridLocationHash{}
    );
    const auto rtaa = rtaa_star_search(
        starts[0], goal, neighbors, heuristic,
        8, 2000, GridLocationHash{}
    );
    const auto lss = lss_lrta_star_search(
        starts[0], goal, neighbors, heuristic,
        8, 2000, GridLocationHash{}
    );
    assert(lrta && rta && rtaa && lss);
    assert(!lrta.learned_heuristic.empty());
    assert(rta.expanded > 0 || starts[0] == goals[0]);
    bool threw = false;
    try{
        static_cast<void>(rtaa_star_search(
            starts[0], goal, neighbors, heuristic,
            0, 10, GridLocationHash{}
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    const auto cbs = bounded_conflict_based_search(
        map, starts, goals, 10000, 40
    );
    const auto enhanced = enhanced_conflict_based_search(
        map, starts, goals, 10000, 40
    );
    const auto ecbs = ecbs_search(
        map, starts, goals, 1.5L, 10000, 40
    );
    assert(cbs && enhanced && ecbs);
    verify_multi_agent(map, cbs.paths, starts, goals);
    verify_multi_agent(map, enhanced.paths, starts, goals);
    verify_multi_agent(map, ecbs.paths, starts, goals);
    assert(enhanced.sum_of_costs == cbs.sum_of_costs);
    assert(ecbs.sum_of_costs * 2 <= cbs.sum_of_costs * 3);

    std::cout << expected_single << ' ' << cbs.sum_of_costs << '\n';
}
