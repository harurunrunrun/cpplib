// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

#include "../../src/approximate/search/grid_path_search.hpp"
#include "../../src/approximate/search/incremental_path_search.hpp"

namespace{

constexpr long long integer_infinity =
    std::numeric_limits<long long>::max() / 4;

using Matrix = std::vector<std::vector<long long>>;

long long exact_graph_distance(
    const Matrix& costs,
    int source,
    int target
){
    std::vector<long long> distance(costs.size(), integer_infinity);
    using Entry = std::pair<long long, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    distance[static_cast<std::size_t>(source)] = 0;
    queue.emplace(0, source);
    while(!queue.empty()){
        const auto [current_distance, vertex] = queue.top();
        queue.pop();
        if(current_distance != distance[static_cast<std::size_t>(vertex)]){
            continue;
        }
        for(int to = 0; to < static_cast<int>(costs.size()); ++to){
            const long long cost = costs[static_cast<std::size_t>(vertex)]
                [static_cast<std::size_t>(to)];
            if(cost == integer_infinity) continue;
            if(current_distance + cost
               < distance[static_cast<std::size_t>(to)]){
                distance[static_cast<std::size_t>(to)] =
                    current_distance + cost;
                queue.emplace(
                    distance[static_cast<std::size_t>(to)], to
                );
            }
        }
    }
    return distance[static_cast<std::size_t>(target)];
}

double exact_grid_distance(
    const GridMap& grid,
    GridPoint source,
    GridPoint target,
    GridConnectivity connectivity,
    bool forbid_diagonal_corner_cut
){
    const std::size_t size =
        static_cast<std::size_t>(grid.row_count())
        * static_cast<std::size_t>(grid.column_count());
    const double infinity = std::numeric_limits<double>::infinity();
    std::vector<double> distance(size, infinity);
    using Entry = std::pair<double, GridPoint>;
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            return lhs.first > rhs.first;
        }
    };
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    if(!grid.is_passable(source) || !grid.is_passable(target)) return infinity;
    distance[grid_path_search_internal::flat_index(grid, source)] = 0.0;
    queue.emplace(0.0, source);
    while(!queue.empty()){
        const auto [current_distance, point] = queue.top();
        queue.pop();
        const std::size_t point_index =
            grid_path_search_internal::flat_index(grid, point);
        if(current_distance != distance[point_index]) continue;
        for(const GridPoint direction :
            grid_path_search_internal::directions){
            if(!grid_path_search_internal::direction_enabled(
                   direction, connectivity
               )){
                continue;
            }
            const GridPoint next =
                grid_path_search_internal::add(point, direction);
            if(!grid.is_passable(next)) continue;
            if(forbid_diagonal_corner_cut
               && direction.row != 0 && direction.column != 0
               && (!grid.is_passable(
                       GridPoint{point.row + direction.row, point.column}
                   )
                   || !grid.is_passable(
                       GridPoint{point.row, point.column + direction.column}
                   ))){
                continue;
            }
            const double candidate =
                current_distance
                + grid_path_search_internal::segment_cost(point, next);
            const std::size_t next_index =
                grid_path_search_internal::flat_index(grid, next);
            if(candidate < distance[next_index]){
                distance[next_index] = candidate;
                queue.emplace(candidate, next);
            }
        }
    }
    return distance[grid_path_search_internal::flat_index(grid, target)];
}

void assert_close(double actual, double expected){
    assert(std::abs(actual - expected)
           <= 1e-9 * std::max({1.0, std::abs(actual), std::abs(expected)}));
}

void check_grid_result(
    const GridMap& grid,
    const GridPathResult& result,
    GridPoint start,
    GridPoint goal,
    double expected
){
    const bool reachable = std::isfinite(expected);
    assert(static_cast<bool>(result) == reachable);
    if(!reachable) return;
    assert(result.path.front() == start);
    assert(result.path.back() == goal);
    double cost = 0.0;
    for(std::size_t index = 1; index < result.path.size(); ++index){
        const GridPoint first = result.path[index - 1];
        const GridPoint second = result.path[index];
        assert(grid.is_passable(second));
        assert(std::max(
                   std::abs(first.row - second.row),
                   std::abs(first.column - second.column)
               ) == 1);
        cost += grid_path_search_internal::segment_cost(first, second);
    }
    assert_close(cost, *result.cost);
    assert_close(*result.cost, expected);
}

void randomized_grid_tests(){
    std::mt19937 random(2026072907U);
    for(int repetition = 0; repetition < 350; ++repetition){
        const int rows = 1 + static_cast<int>(random() % 10U);
        const int columns = 1 + static_cast<int>(random() % 10U);
        GridMap grid(rows, columns);
        for(int row = 0; row < rows; ++row){
            for(int column = 0; column < columns; ++column){
                if(random() % 4U == 0U){
                    grid.set_passable(GridPoint{row, column}, false);
                }
            }
        }
        const GridPoint start{
            static_cast<int>(random() % static_cast<unsigned>(rows)),
            static_cast<int>(random() % static_cast<unsigned>(columns))
        };
        const GridPoint goal{
            static_cast<int>(random() % static_cast<unsigned>(rows)),
            static_cast<int>(random() % static_cast<unsigned>(columns))
        };
        grid.set_passable(start, true);
        grid.set_passable(goal, true);
        for(const GridConnectivity connectivity :
            {GridConnectivity::four, GridConnectivity::eight_corner_cut}){
            const double expected =
                exact_grid_distance(grid, start, goal, connectivity, false);
            const auto jps =
                jump_point_search(grid, start, goal, connectivity);
            const JumpPointSearchPlus prepared(grid, connectivity);
            const auto jps_plus = prepared.search(start, goal);
            check_grid_result(grid, jps, start, goal, expected);
            check_grid_result(grid, jps_plus, start, goal, expected);
        }

        const double discrete = exact_grid_distance(
            grid,
            start,
            goal,
            GridConnectivity::eight_corner_cut,
            true
        );
        for(const GridPathResult& result :
            {theta_star_search(grid, start, goal),
             lazy_theta_star_search(grid, start, goal)}){
            assert(static_cast<bool>(result) == std::isfinite(discrete));
            if(!result) continue;
            assert(result.path.front() == start && result.path.back() == goal);
            double path_cost = 0.0;
            for(std::size_t index = 1; index < result.path.size(); ++index){
                assert(grid_line_of_sight(
                    grid, result.path[index - 1], result.path[index]
                ));
                path_cost += grid_path_search_internal::segment_cost(
                    result.path[index - 1], result.path[index]
                );
            }
            assert_close(path_cost, *result.cost);
            assert(*result.cost <= discrete + 1e-9);
        }
    }
}

template<class Planner>
void load_graph(Planner& planner, const Matrix& costs){
    for(int from = 0; from < static_cast<int>(costs.size()); ++from){
        for(int to = 0; to < static_cast<int>(costs.size()); ++to){
            if(costs[static_cast<std::size_t>(from)]
                    [static_cast<std::size_t>(to)] != integer_infinity){
                planner.set_edge(
                    from,
                    to,
                    costs[static_cast<std::size_t>(from)]
                        [static_cast<std::size_t>(to)]
                );
            }
        }
    }
}

template<class Result>
void check_incremental_result(const Result& result, long long expected){
    const bool reachable = expected != integer_infinity;
    assert(static_cast<bool>(result) == reachable);
    if(reachable) assert(*result.cost == expected);
}

void randomized_incremental_tests(){
    std::mt19937 random(2026072911U);
    const auto zero = [](int, int){ return 0LL; };
    for(int repetition = 0; repetition < 90; ++repetition){
        const int n = 2 + static_cast<int>(random() % 15U);
        int start = static_cast<int>(random() % static_cast<unsigned>(n));
        const int lpa_start = start;
        const int goal = static_cast<int>(random() % static_cast<unsigned>(n));
        Matrix costs(
            static_cast<std::size_t>(n),
            std::vector<long long>(static_cast<std::size_t>(n), integer_infinity)
        );
        for(int from = 0; from < n; ++from){
            for(int to = 0; to < n; ++to){
                if(from != to && random() % 5U == 0U){
                    costs[static_cast<std::size_t>(from)]
                        [static_cast<std::size_t>(to)] =
                        1 + static_cast<long long>(random() % 30U);
                }
            }
        }
        LifelongPlanningAStar<long long> lpa(n, start, goal, zero);
        LPAStar<long long> lpa_alias(n, start, goal, zero);
        DStar<long long> dstar(n, start, goal);
        FocusedDStar<long long> focused(n, start, goal, zero);
        DStarLite<long long> lite(n, start, goal, zero);
        load_graph(lpa, costs);
        load_graph(lpa_alias, costs);
        load_graph(dstar, costs);
        load_graph(focused, costs);
        load_graph(lite, costs);
        for(int update = 0; update < 45; ++update){
            const long long fixed_expected =
                exact_graph_distance(costs, lpa_start, goal);
            const long long moving_expected =
                exact_graph_distance(costs, start, goal);
            check_incremental_result(lpa.replan(), fixed_expected);
            check_incremental_result(lpa_alias.replan(), fixed_expected);
            check_incremental_result(dstar.replan(), moving_expected);
            check_incremental_result(focused.replan(), moving_expected);
            check_incremental_result(lite.replan(), moving_expected);

            if(update == 22){
                start = static_cast<int>(
                    random() % static_cast<unsigned>(n)
                );
                dstar.move_start(start);
                focused.move_start(start);
                lite.move_start(start);
                continue;
            }
            const int from =
                static_cast<int>(random() % static_cast<unsigned>(n));
            int to = static_cast<int>(random() % static_cast<unsigned>(n));
            if(to == from) to = (to + 1) % n;
            if(random() % 4U == 0U){
                costs[static_cast<std::size_t>(from)]
                    [static_cast<std::size_t>(to)] = integer_infinity;
                lpa.remove_edge(from, to);
                lpa_alias.remove_edge(from, to);
                dstar.remove_edge(from, to);
                focused.remove_edge(from, to);
                lite.remove_edge(from, to);
            }else{
                const long long cost =
                    1 + static_cast<long long>(random() % 30U);
                costs[static_cast<std::size_t>(from)]
                    [static_cast<std::size_t>(to)] = cost;
                lpa.set_edge(from, to, cost);
                lpa_alias.set_edge(from, to, cost);
                dstar.set_edge(from, to, cost);
                focused.set_edge(from, to, cost);
                lite.set_edge(from, to, cost);
            }
        }
    }
}

void field_d_star_tests(){
    std::mt19937 random(2026072917U);
    for(int repetition = 0; repetition < 45; ++repetition){
        constexpr int rows = 8;
        constexpr int columns = 9;
        GridMap grid(rows, columns);
        const GridPoint start{0, 0};
        const GridPoint goal{rows - 1, columns - 1};
        FieldDStar field(grid, start, goal);
        for(int update = 0; update < 35; ++update){
            const double expected = exact_grid_distance(
                grid,
                start,
                goal,
                GridConnectivity::eight_corner_cut,
                true
            );
            const auto result = field.replan();
            assert(static_cast<bool>(result) == std::isfinite(expected));
            if(result){
                assert_close(*result.discrete_cost, expected);
                assert(*result.interpolated_cost
                       <= *result.discrete_cost + 1e-9);
                for(std::size_t index = 1;
                    index < result.field_path.size();
                    ++index){
                    assert(grid_line_of_sight(
                        grid,
                        result.field_path[index - 1],
                        result.field_path[index]
                    ));
                }
            }
            GridPoint changed{
                static_cast<int>(random() % rows),
                static_cast<int>(random() % columns)
            };
            if(changed == start || changed == goal) continue;
            const bool passable = !grid.is_passable(changed);
            grid.set_passable(changed, passable);
            field.set_passable(changed, passable);
        }
    }
}

void exception_tests(){
    bool threw = false;
    try{
        LifelongPlanningAStar<int> planner(2, 0, 1);
        planner.set_edge(0, 1, 0);
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

void self_test(){
    randomized_grid_tests();
    randomized_incremental_tests();
    field_d_star_tests();
    exception_tests();
}

} // namespace

int main(){
    self_test();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    const auto zero = [](int, int){ return 0LL; };
    for(int case_index = 0; case_index < test_count; ++case_index){
        int vertex_count;
        int edge_count;
        int start;
        int goal;
        int update_count;
        std::cin >> vertex_count >> edge_count >> start >> goal >> update_count;
        Matrix costs(
            static_cast<std::size_t>(vertex_count),
            std::vector<long long>(
                static_cast<std::size_t>(vertex_count), integer_infinity
            )
        );
        LifelongPlanningAStar<long long> lpa(
            vertex_count, start, goal, zero
        );
        LPAStar<long long> alias(vertex_count, start, goal, zero);
        DStar<long long> dstar(vertex_count, start, goal);
        FocusedDStar<long long> focused(
            vertex_count, start, goal, zero
        );
        DStarLite<long long> lite(vertex_count, start, goal, zero);
        for(int edge_index = 0; edge_index < edge_count; ++edge_index){
            int from;
            int to;
            long long cost;
            std::cin >> from >> to >> cost;
            costs[static_cast<std::size_t>(from)]
                [static_cast<std::size_t>(to)] = cost;
            lpa.set_edge(from, to, cost);
            alias.set_edge(from, to, cost);
            dstar.set_edge(from, to, cost);
            focused.set_edge(from, to, cost);
            lite.set_edge(from, to, cost);
        }
        for(int phase = 0; phase <= update_count; ++phase){
            const auto a = lpa.replan();
            const auto b = alias.replan();
            const auto c = dstar.replan();
            const auto d = focused.replan();
            const auto e = lite.replan();
            const auto value = [](const auto& result){
                return result.cost.value_or(-1);
            };
            std::cout << value(a) << ' ' << value(b) << ' '
                      << value(c) << ' ' << value(d) << ' '
                      << value(e) << '\n';
            if(phase == update_count) break;
            int operation;
            int from;
            int to;
            long long cost;
            std::cin >> operation >> from >> to >> cost;
            if(operation == 0){
                lpa.remove_edge(from, to);
                alias.remove_edge(from, to);
                dstar.remove_edge(from, to);
                focused.remove_edge(from, to);
                lite.remove_edge(from, to);
            }else{
                lpa.set_edge(from, to, cost);
                alias.set_edge(from, to, cost);
                dstar.set_edge(from, to, cost);
                focused.set_edge(from, to, cost);
                lite.set_edge(from, to, cost);
            }
        }
    }
}
