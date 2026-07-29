#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_GRID_PATH_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_GRID_PATH_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct GridPoint{
    int row = 0;
    int column = 0;

    friend bool operator==(const GridPoint&, const GridPoint&) = default;
};

enum class GridConnectivity{
    four,
    eight_corner_cut
};

class GridMap{
    int row_count_ = 0;
    int column_count_ = 0;
    std::vector<std::uint8_t> passable_;

    std::size_t index(GridPoint point) const noexcept{
        return static_cast<std::size_t>(point.row)
            * static_cast<std::size_t>(column_count_)
            + static_cast<std::size_t>(point.column);
    }

public:
    GridMap() = default;

    GridMap(int row_count, int column_count, bool initially_passable = true)
        : row_count_(row_count),
          column_count_(column_count),
          passable_(
              static_cast<std::size_t>(std::max(row_count, 0))
                  * static_cast<std::size_t>(std::max(column_count, 0)),
              static_cast<std::uint8_t>(initially_passable)
          ){
        if(row_count < 0 || column_count < 0){
            throw std::invalid_argument("GridMap dimensions must be non-negative");
        }
    }

    explicit GridMap(const std::vector<std::string>& rows, char blocked = '#')
        : row_count_(static_cast<int>(rows.size())),
          column_count_(rows.empty() ? 0 : static_cast<int>(rows.front().size())),
          passable_(
              static_cast<std::size_t>(row_count_)
                  * static_cast<std::size_t>(column_count_),
              0
          ){
        for(int row = 0; row < row_count_; ++row){
            if(static_cast<int>(rows[static_cast<std::size_t>(row)].size())
               != column_count_){
                throw std::invalid_argument("GridMap rows must have equal lengths");
            }
            for(int column = 0; column < column_count_; ++column){
                passable_[index(GridPoint{row, column})] =
                    static_cast<std::uint8_t>(
                        rows[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(column)] != blocked
                    );
            }
        }
    }

    int row_count() const noexcept{ return row_count_; }
    int column_count() const noexcept{ return column_count_; }

    bool contains(GridPoint point) const noexcept{
        return 0 <= point.row && point.row < row_count_
            && 0 <= point.column && point.column < column_count_;
    }

    bool is_passable(GridPoint point) const noexcept{
        return contains(point) && passable_[index(point)] != 0;
    }

    void set_passable(GridPoint point, bool passable){
        if(!contains(point)){
            throw std::out_of_range("GridMap::set_passable point is outside the grid");
        }
        passable_[index(point)] = static_cast<std::uint8_t>(passable);
    }
};

struct GridPathResult{
    std::optional<double> cost;
    std::vector<GridPoint> path;
    std::size_t expanded = 0;
    std::size_t line_of_sight_checks = 0;

    explicit operator bool() const noexcept{ return cost.has_value(); }
};

namespace grid_path_search_internal{

inline constexpr std::array<GridPoint, 8> directions{{
    {-1, 0}, {1, 0}, {0, -1}, {0, 1},
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
}};

inline int sign(int value) noexcept{
    return (0 < value) - (value < 0);
}

inline GridPoint add(GridPoint point, GridPoint direction) noexcept{
    return GridPoint{
        point.row + direction.row,
        point.column + direction.column
    };
}

inline GridPoint subtract(GridPoint lhs, GridPoint rhs) noexcept{
    return GridPoint{lhs.row - rhs.row, lhs.column - rhs.column};
}

inline GridPoint normalized_direction(GridPoint from, GridPoint to) noexcept{
    return GridPoint{sign(to.row - from.row), sign(to.column - from.column)};
}

inline double segment_cost(GridPoint first, GridPoint second) noexcept{
    return std::hypot(
        static_cast<double>(first.row - second.row),
        static_cast<double>(first.column - second.column)
    );
}

inline double octile(GridPoint first, GridPoint second) noexcept{
    const int row_delta = std::abs(first.row - second.row);
    const int column_delta = std::abs(first.column - second.column);
    const int diagonal = std::min(row_delta, column_delta);
    return static_cast<double>(row_delta + column_delta - 2 * diagonal)
        + std::sqrt(2.0) * static_cast<double>(diagonal);
}

inline bool direction_enabled(GridPoint direction, GridConnectivity connectivity){
    return connectivity == GridConnectivity::eight_corner_cut
        || direction.row == 0 || direction.column == 0;
}

inline bool can_step(
    const GridMap& grid,
    GridPoint from,
    GridPoint direction,
    GridConnectivity connectivity
){
    if(!direction_enabled(direction, connectivity)) return false;
    return grid.is_passable(from) && grid.is_passable(add(from, direction));
}

inline std::size_t flat_index(const GridMap& grid, GridPoint point){
    return static_cast<std::size_t>(point.row)
        * static_cast<std::size_t>(grid.column_count())
        + static_cast<std::size_t>(point.column);
}

inline bool has_forced_neighbor(
    const GridMap& grid,
    GridPoint point,
    GridPoint direction,
    GridConnectivity connectivity
){
    const int dr = direction.row;
    const int dc = direction.column;
    if(connectivity == GridConnectivity::four){
        if(dc != 0){
            for(const int side : {-1, 1}){
                if(grid.is_passable(GridPoint{point.row + side, point.column})
                   && !grid.is_passable(
                       GridPoint{point.row + side, point.column - dc}
                   )){
                    return true;
                }
            }
        }else{
            for(const int side : {-1, 1}){
                if(grid.is_passable(GridPoint{point.row, point.column + side})
                   && !grid.is_passable(
                       GridPoint{point.row - dr, point.column + side}
                   )){
                    return true;
                }
            }
        }
        return false;
    }
    if(dr != 0 && dc != 0){
        return (
            !grid.is_passable(GridPoint{point.row - dr, point.column})
            && grid.is_passable(GridPoint{point.row - dr, point.column + dc})
        ) || (
            !grid.is_passable(GridPoint{point.row, point.column - dc})
            && grid.is_passable(GridPoint{point.row + dr, point.column - dc})
        );
    }
    if(dc != 0){
        return (
            !grid.is_passable(GridPoint{point.row - 1, point.column})
            && grid.is_passable(GridPoint{point.row - 1, point.column + dc})
        ) || (
            !grid.is_passable(GridPoint{point.row + 1, point.column})
            && grid.is_passable(GridPoint{point.row + 1, point.column + dc})
        );
    }
    return (
        !grid.is_passable(GridPoint{point.row, point.column - 1})
        && grid.is_passable(GridPoint{point.row + dr, point.column - 1})
    ) || (
        !grid.is_passable(GridPoint{point.row, point.column + 1})
        && grid.is_passable(GridPoint{point.row + dr, point.column + 1})
    );
}

inline std::vector<GridPoint> pruned_directions(
    const GridMap& grid,
    GridPoint point,
    std::optional<GridPoint> parent,
    GridConnectivity connectivity
){
    std::vector<GridPoint> result;
    if(!parent){
        for(const GridPoint direction : directions){
            if(can_step(grid, point, direction, connectivity)){
                result.push_back(direction);
            }
        }
        return result;
    }
    const GridPoint direction = normalized_direction(*parent, point);
    const int dr = direction.row;
    const int dc = direction.column;
    const auto add_if_legal = [&](GridPoint candidate){
        if(can_step(grid, point, candidate, connectivity)
           && std::find(result.begin(), result.end(), candidate) == result.end()){
            result.push_back(candidate);
        }
    };
    if(connectivity == GridConnectivity::four){
        add_if_legal(direction);
        if(dc != 0){
            add_if_legal(GridPoint{-1, 0});
            add_if_legal(GridPoint{1, 0});
        }else{
            add_if_legal(GridPoint{0, -1});
            add_if_legal(GridPoint{0, 1});
        }
        return result;
    }
    if(dr != 0 && dc != 0){
        add_if_legal(GridPoint{dr, dc});
        add_if_legal(GridPoint{dr, 0});
        add_if_legal(GridPoint{0, dc});
        if(!grid.is_passable(GridPoint{point.row - dr, point.column})){
            add_if_legal(GridPoint{-dr, dc});
        }
        if(!grid.is_passable(GridPoint{point.row, point.column - dc})){
            add_if_legal(GridPoint{dr, -dc});
        }
    }else if(dc != 0){
        add_if_legal(direction);
        if(!grid.is_passable(GridPoint{point.row - 1, point.column})){
            add_if_legal(GridPoint{-1, dc});
        }
        if(!grid.is_passable(GridPoint{point.row + 1, point.column})){
            add_if_legal(GridPoint{1, dc});
        }
    }else{
        add_if_legal(direction);
        if(!grid.is_passable(GridPoint{point.row, point.column - 1})){
            add_if_legal(GridPoint{dr, -1});
        }
        if(!grid.is_passable(GridPoint{point.row, point.column + 1})){
            add_if_legal(GridPoint{dr, 1});
        }
    }
    return result;
}

inline std::optional<GridPoint> jump(
    const GridMap& grid,
    GridPoint point,
    GridPoint direction,
    GridPoint goal,
    GridConnectivity connectivity
){
    if(connectivity == GridConnectivity::four){
        if(!can_step(grid, point, direction, connectivity)) return std::nullopt;
        return add(point, direction);
    }
    while(can_step(grid, point, direction, connectivity)){
        point = add(point, direction);
        if(point == goal || has_forced_neighbor(grid, point, direction, connectivity)){
            return point;
        }
        if(connectivity == GridConnectivity::four){
            const GridPoint toward_goal = normalized_direction(point, goal);
            const bool aligned =
                (direction.row != 0 && point.row == goal.row
                 && toward_goal.column != 0)
                || (direction.column != 0 && point.column == goal.column
                    && toward_goal.row != 0);
            if(aligned){
                GridPoint cursor = point;
                while(cursor != goal
                      && can_step(grid, cursor, toward_goal, connectivity)){
                    cursor = add(cursor, toward_goal);
                }
                if(cursor == goal) return point;
            }
        }
        if(connectivity == GridConnectivity::eight_corner_cut
           && direction.row != 0 && direction.column != 0){
            if(jump(
                   grid, point, GridPoint{direction.row, 0}, goal, connectivity
               )
               || jump(
                   grid, point, GridPoint{0, direction.column}, goal, connectivity
               )){
                return point;
            }
        }
    }
    return std::nullopt;
}

inline std::vector<GridPoint> expand_jump_path(
    const std::vector<GridPoint>& jump_path
){
    if(jump_path.empty()) return {};
    std::vector<GridPoint> path{jump_path.front()};
    for(std::size_t index = 1; index < jump_path.size(); ++index){
        GridPoint current = jump_path[index - 1];
        const GridPoint direction =
            normalized_direction(current, jump_path[index]);
        while(current != jump_path[index]){
            current = add(current, direction);
            path.push_back(current);
        }
    }
    return path;
}

template<class JumpFunction>
GridPathResult jps_search_impl(
    const GridMap& grid,
    GridPoint start,
    GridPoint goal,
    GridConnectivity connectivity,
    JumpFunction jump_function
){
    GridPathResult result;
    if(!grid.is_passable(start) || !grid.is_passable(goal)) return result;
    const std::size_t vertex_count =
        static_cast<std::size_t>(grid.row_count())
        * static_cast<std::size_t>(grid.column_count());
    const double infinity = std::numeric_limits<double>::infinity();
    std::vector<double> distance(vertex_count, infinity);
    std::vector<GridPoint> parent(vertex_count);
    std::vector<std::uint8_t> has_parent(vertex_count, 0);
    struct Entry{
        double priority;
        double distance;
        std::size_t order;
        GridPoint point;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(lhs.priority != rhs.priority) return lhs.priority > rhs.priority;
            return lhs.order > rhs.order;
        }
    };
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    const std::size_t start_index = flat_index(grid, start);
    distance[start_index] = 0.0;
    std::size_t order = 0;
    queue.push(Entry{octile(start, goal), 0.0, order++, start});
    while(!queue.empty()){
        const Entry entry = queue.top();
        queue.pop();
        const std::size_t current_index = flat_index(grid, entry.point);
        if(entry.distance != distance[current_index]) continue;
        ++result.expanded;
        if(entry.point == goal){
            std::vector<GridPoint> jump_path;
            GridPoint current = goal;
            while(true){
                jump_path.push_back(current);
                const std::size_t current_flat = flat_index(grid, current);
                if(!has_parent[current_flat]) break;
                current = parent[current_flat];
            }
            std::reverse(jump_path.begin(), jump_path.end());
            result.cost = entry.distance;
            result.path = expand_jump_path(jump_path);
            return result;
        }
        const std::optional<GridPoint> previous =
            has_parent[current_index]
                ? std::optional<GridPoint>(parent[current_index])
                : std::nullopt;
        for(const GridPoint direction :
            pruned_directions(grid, entry.point, previous, connectivity)){
            const auto next = jump_function(entry.point, direction, goal);
            if(!next) continue;
            const std::size_t next_index = flat_index(grid, *next);
            const double candidate =
                entry.distance + segment_cost(entry.point, *next);
            if(!(candidate < distance[next_index])) continue;
            distance[next_index] = candidate;
            parent[next_index] = entry.point;
            has_parent[next_index] = 1;
            queue.push(Entry{
                candidate + octile(*next, goal),
                candidate,
                order++,
                *next
            });
        }
    }
    return result;
}

inline bool line_of_sight_impl(
    const GridMap& grid,
    GridPoint first,
    GridPoint second
){
    if(!grid.is_passable(first) || !grid.is_passable(second)) return false;
    int row = first.row;
    int column = first.column;
    const int row_delta = second.row - first.row;
    const int column_delta = second.column - first.column;
    const int row_steps = std::abs(row_delta);
    const int column_steps = std::abs(column_delta);
    const int row_direction = sign(row_delta);
    const int column_direction = sign(column_delta);
    int row_index = 0;
    int column_index = 0;
    while(row_index < row_steps || column_index < column_steps){
        const long long row_decision =
            static_cast<long long>(1 + 2 * row_index) * column_steps;
        const long long column_decision =
            static_cast<long long>(1 + 2 * column_index) * row_steps;
        if(row_decision == column_decision){
            if(row_direction != 0 && column_direction != 0
               && (!grid.is_passable(GridPoint{row + row_direction, column})
                   || !grid.is_passable(
                       GridPoint{row, column + column_direction}
                   ))){
                return false;
            }
            row += row_direction;
            column += column_direction;
            ++row_index;
            ++column_index;
        }else if(row_decision < column_decision){
            row += row_direction;
            ++row_index;
        }else{
            column += column_direction;
            ++column_index;
        }
        if(!grid.is_passable(GridPoint{row, column})) return false;
    }
    return true;
}

template<bool Lazy>
GridPathResult theta_search_impl(
    const GridMap& grid,
    GridPoint start,
    GridPoint goal
){
    GridPathResult result;
    if(!grid.is_passable(start) || !grid.is_passable(goal)) return result;
    const std::size_t vertex_count =
        static_cast<std::size_t>(grid.row_count())
        * static_cast<std::size_t>(grid.column_count());
    const double infinity = std::numeric_limits<double>::infinity();
    std::vector<double> distance(vertex_count, infinity);
    std::vector<GridPoint> parent(vertex_count);
    std::vector<std::uint8_t> closed(vertex_count, 0);
    struct Entry{
        double priority;
        double distance;
        std::size_t order;
        GridPoint point;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(lhs.priority != rhs.priority) return lhs.priority > rhs.priority;
            return lhs.order > rhs.order;
        }
    };
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    const std::size_t start_index = flat_index(grid, start);
    distance[start_index] = 0.0;
    parent[start_index] = start;
    std::size_t order = 0;
    queue.push(Entry{segment_cost(start, goal), 0.0, order++, start});
    while(!queue.empty()){
        Entry entry = queue.top();
        queue.pop();
        const std::size_t current_index = flat_index(grid, entry.point);
        if(entry.distance != distance[current_index] || closed[current_index]){
            continue;
        }
        if constexpr(Lazy){
            if(parent[current_index] != entry.point){
                ++result.line_of_sight_checks;
                if(!line_of_sight_impl(
                       grid, parent[current_index], entry.point
                   )){
                    double best = infinity;
                    GridPoint best_parent = entry.point;
                    for(const GridPoint direction : directions){
                        const GridPoint candidate_parent =
                            add(entry.point, direction);
                        if(direction.row != 0 && direction.column != 0
                           && !line_of_sight_impl(
                               grid, entry.point, candidate_parent
                           )){
                            continue;
                        }
                        if(!can_step(
                               grid,
                               entry.point,
                               direction,
                               GridConnectivity::eight_corner_cut
                           )){
                            continue;
                        }
                        const std::size_t candidate_index =
                            flat_index(grid, candidate_parent);
                        if(!closed[candidate_index]) continue;
                        const double candidate =
                            distance[candidate_index]
                            + segment_cost(candidate_parent, entry.point);
                        if(candidate < best){
                            best = candidate;
                            best_parent = candidate_parent;
                        }
                    }
                    distance[current_index] = best;
                    parent[current_index] = best_parent;
                    entry.distance = best;
                }
            }
        }
        closed[current_index] = 1;
        ++result.expanded;
        if(entry.point == goal){
            std::vector<GridPoint> path;
            GridPoint current = goal;
            for(std::size_t guard = 0; guard <= vertex_count; ++guard){
                path.push_back(current);
                if(current == start) break;
                current = parent[flat_index(grid, current)];
            }
            if(path.back() != start) return GridPathResult{};
            std::reverse(path.begin(), path.end());
            result.cost = distance[current_index];
            result.path = std::move(path);
            return result;
        }
        for(const GridPoint direction : directions){
            if(!can_step(
                   grid,
                   entry.point,
                   direction,
                   GridConnectivity::eight_corner_cut
               )){
                continue;
            }
            const GridPoint next = add(entry.point, direction);
            if(direction.row != 0 && direction.column != 0
               && !line_of_sight_impl(grid, entry.point, next)){
                continue;
            }
            const std::size_t next_index = flat_index(grid, next);
            if(closed[next_index]) continue;
            GridPoint candidate_parent = entry.point;
            double candidate = distance[current_index] + segment_cost(entry.point, next);
            if constexpr(Lazy){
                candidate_parent = parent[current_index];
                candidate =
                    distance[flat_index(grid, candidate_parent)]
                    + segment_cost(candidate_parent, next);
            }else{
                ++result.line_of_sight_checks;
                const GridPoint grandparent = parent[current_index];
                if(line_of_sight_impl(grid, grandparent, next)){
                    const double through_parent =
                        distance[flat_index(grid, grandparent)]
                        + segment_cost(grandparent, next);
                    if(through_parent < candidate){
                        candidate = through_parent;
                        candidate_parent = grandparent;
                    }
                }
            }
            if(!(candidate < distance[next_index])) continue;
            distance[next_index] = candidate;
            parent[next_index] = candidate_parent;
            queue.push(Entry{
                candidate + segment_cost(next, goal),
                candidate,
                order++,
                next
            });
        }
    }
    return result;
}

} // namespace grid_path_search_internal

inline bool grid_line_of_sight(
    const GridMap& grid,
    GridPoint first,
    GridPoint second
){
    return grid_path_search_internal::line_of_sight_impl(grid, first, second);
}

inline GridPathResult jump_point_search(
    const GridMap& grid,
    GridPoint start,
    GridPoint goal,
    GridConnectivity connectivity
){
    return grid_path_search_internal::jps_search_impl(
        grid,
        start,
        goal,
        connectivity,
        [&](GridPoint point, GridPoint direction, GridPoint fixed_goal){
            return grid_path_search_internal::jump(
                grid, point, direction, fixed_goal, connectivity
            );
        }
    );
}

class JumpPointSearchPlus{
    GridMap grid_;
    GridConnectivity connectivity_;
    std::vector<std::array<int, 8>> run_length_;
    std::vector<std::array<int, 8>> jump_distance_;

    static int direction_index(GridPoint direction){
        for(int index = 0; index < 8; ++index){
            if(grid_path_search_internal::directions[
                   static_cast<std::size_t>(index)
               ] == direction){
                return index;
            }
        }
        throw std::logic_error("invalid JPS direction");
    }

    int run(GridPoint point, GridPoint direction) const{
        return run_length_[grid_path_search_internal::flat_index(grid_, point)]
            [static_cast<std::size_t>(direction_index(direction))];
    }

    int event(GridPoint point, GridPoint direction) const{
        return jump_distance_[grid_path_search_internal::flat_index(grid_, point)]
            [static_cast<std::size_t>(direction_index(direction))];
    }

    std::optional<GridPoint> jump_plus(
        GridPoint point,
        GridPoint direction,
        GridPoint goal
    ) const{
        const int maximum = run(point, direction);
        if(connectivity_ == GridConnectivity::four){
            if(maximum == 0) return std::nullopt;
            return grid_path_search_internal::add(point, direction);
        }
        if(maximum == 0) return std::nullopt;
        int best = event(point, direction);
        const auto consider = [&](int steps, int& current_best){
            if(0 < steps && steps <= maximum
               && (current_best == 0 || steps < current_best)){
                current_best = steps;
            }
        };
        const int row_delta = goal.row - point.row;
        const int column_delta = goal.column - point.column;
        if(direction.row == 0 && row_delta == 0
           && grid_path_search_internal::sign(column_delta) == direction.column){
            consider(std::abs(column_delta), best);
        }else if(direction.column == 0 && column_delta == 0
                 && grid_path_search_internal::sign(row_delta) == direction.row){
            consider(std::abs(row_delta), best);
        }else if(connectivity_ == GridConnectivity::four){
            if(direction.row != 0 && row_delta * direction.row > 0
               && goal.column != point.column){
                const int steps = std::abs(row_delta);
                const GridPoint turn{
                    point.row + steps * direction.row,
                    point.column
                };
                const GridPoint straight{
                    0,
                    grid_path_search_internal::sign(goal.column - turn.column)
                };
                if(steps <= maximum
                   && run(turn, straight)
                        >= std::abs(goal.column - turn.column)){
                    consider(steps, best);
                }
            }
            if(direction.column != 0 && column_delta * direction.column > 0
               && goal.row != point.row){
                const int steps = std::abs(column_delta);
                const GridPoint turn{
                    point.row,
                    point.column + steps * direction.column
                };
                const GridPoint straight{
                    grid_path_search_internal::sign(goal.row - turn.row),
                    0
                };
                if(steps <= maximum
                   && run(turn, straight)
                        >= std::abs(goal.row - turn.row)){
                    consider(steps, best);
                }
            }
        }else if(direction.row != 0 && direction.column != 0){
            if(std::abs(row_delta) == std::abs(column_delta)
               && grid_path_search_internal::sign(row_delta) == direction.row
               && grid_path_search_internal::sign(column_delta)
                    == direction.column){
                consider(std::abs(row_delta), best);
            }
            const auto consider_turn = [&](int steps, GridPoint straight){
                if(steps <= 0 || steps > maximum) return;
                const GridPoint turn{
                    point.row + steps * direction.row,
                    point.column + steps * direction.column
                };
                const int straight_steps =
                    straight.row == 0
                        ? std::abs(goal.column - turn.column)
                        : std::abs(goal.row - turn.row);
                if(straight_steps > 0
                   && grid_path_search_internal::normalized_direction(turn, goal)
                        == straight
                   && run(turn, straight) >= straight_steps){
                    consider(steps, best);
                }
            };
            if(row_delta * direction.row > 0){
                consider_turn(
                    std::abs(row_delta),
                    GridPoint{
                        0,
                        grid_path_search_internal::sign(
                            goal.column
                            - (point.column
                               + std::abs(row_delta) * direction.column)
                        )
                    }
                );
            }
            if(column_delta * direction.column > 0){
                consider_turn(
                    std::abs(column_delta),
                    GridPoint{
                        grid_path_search_internal::sign(
                            goal.row
                            - (point.row
                               + std::abs(column_delta) * direction.row)
                        ),
                        0
                    }
                );
            }
        }
        if(best == 0) return std::nullopt;
        return GridPoint{
            point.row + best * direction.row,
            point.column + best * direction.column
        };
    }

    void preprocess_direction(int direction_id){
        const GridPoint direction =
            grid_path_search_internal::directions[
                static_cast<std::size_t>(direction_id)
            ];
        if(!grid_path_search_internal::direction_enabled(
               direction, connectivity_
           )){
            return;
        }
        const int row_begin = direction.row > 0 ? grid_.row_count() - 1 : 0;
        const int row_end = direction.row > 0 ? -1 : grid_.row_count();
        const int row_step = direction.row > 0 ? -1 : 1;
        const int column_begin =
            direction.column > 0 ? grid_.column_count() - 1 : 0;
        const int column_end =
            direction.column > 0 ? -1 : grid_.column_count();
        const int column_step = direction.column > 0 ? -1 : 1;
        for(int row = row_begin; row != row_end; row += row_step){
            for(int column = column_begin;
                column != column_end;
                column += column_step){
                const GridPoint point{row, column};
                if(!grid_.is_passable(point)) continue;
                const GridPoint next =
                    grid_path_search_internal::add(point, direction);
                if(!grid_path_search_internal::can_step(
                       grid_, point, direction, connectivity_
                   )){
                    continue;
                }
                const std::size_t current_index =
                    grid_path_search_internal::flat_index(grid_, point);
                const std::size_t next_index =
                    grid_path_search_internal::flat_index(grid_, next);
                run_length_[current_index][static_cast<std::size_t>(direction_id)] =
                    1 + run_length_[next_index]
                        [static_cast<std::size_t>(direction_id)];
                bool is_event =
                    grid_path_search_internal::has_forced_neighbor(
                        grid_, next, direction, connectivity_
                    );
                if(connectivity_ == GridConnectivity::eight_corner_cut
                   && direction.row != 0 && direction.column != 0){
                    is_event = is_event
                        || event(next, GridPoint{direction.row, 0}) != 0
                        || event(next, GridPoint{0, direction.column}) != 0;
                }
                jump_distance_[current_index]
                    [static_cast<std::size_t>(direction_id)] =
                    is_event
                        ? 1
                        : (jump_distance_[next_index]
                               [static_cast<std::size_t>(direction_id)] == 0
                               ? 0
                               : 1 + jump_distance_[next_index]
                                   [static_cast<std::size_t>(direction_id)]);
            }
        }
    }

public:
    explicit JumpPointSearchPlus(
        GridMap grid,
        GridConnectivity connectivity
    )
        : grid_(std::move(grid)),
          connectivity_(connectivity),
          run_length_(
              static_cast<std::size_t>(grid_.row_count())
                  * static_cast<std::size_t>(grid_.column_count())
          ),
          jump_distance_(run_length_.size()){
        for(int direction = 0; direction < 4; ++direction){
            preprocess_direction(direction);
        }
        for(int direction = 4; direction < 8; ++direction){
            preprocess_direction(direction);
        }
    }

    const GridMap& grid() const noexcept{ return grid_; }
    GridConnectivity connectivity() const noexcept{ return connectivity_; }

    GridPathResult search(GridPoint start, GridPoint goal) const{
        return grid_path_search_internal::jps_search_impl(
            grid_,
            start,
            goal,
            connectivity_,
            [&](GridPoint point, GridPoint direction, GridPoint fixed_goal){
                return jump_plus(point, direction, fixed_goal);
            }
        );
    }
};

inline GridPathResult theta_star_search(
    const GridMap& grid,
    GridPoint start,
    GridPoint goal
){
    return grid_path_search_internal::theta_search_impl<false>(
        grid, start, goal
    );
}

inline GridPathResult lazy_theta_star_search(
    const GridMap& grid,
    GridPoint start,
    GridPoint goal
){
    return grid_path_search_internal::theta_search_impl<true>(
        grid, start, goal
    );
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_GRID_PATH_SEARCH_HPP_INCLUDED
