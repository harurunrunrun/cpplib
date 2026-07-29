#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_ALLOCATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_ALLOCATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <queue>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::graph {

template<class Value>
struct AuctionResult {
    std::vector<std::size_t> bidder_to_item;
    std::vector<std::size_t> item_to_bidder;
    Value total_value{};
    std::size_t bids = 0;
};

namespace allocation_detail {

template<class Value>
[[nodiscard]] std::pair<std::size_t, std::size_t> dimensions(
    const std::vector<std::vector<Value>>& values
){
    static_assert(
        std::is_arithmetic_v<Value> && !std::is_same_v<Value, bool>
    );
    if(values.empty()) return {0, 0};
    const std::size_t item_count = values.front().size();
    for(const auto& row : values){
        if(row.size() != item_count)[[unlikely]]{
            throw std::invalid_argument("valuation row sizes differ");
        }
        if constexpr(std::is_floating_point_v<Value>){
            for(const Value value : row){
                if(!std::isfinite(value))[[unlikely]]{
                    throw std::invalid_argument("valuations must be finite");
                }
            }
        }
    }
    return {values.size(), item_count};
}

template<class Value>
void require_auction_epsilon(Value epsilon){
    static_assert(std::is_floating_point_v<Value>);
    if(!std::isfinite(epsilon) || !(epsilon > Value{}))[[unlikely]]{
        throw std::invalid_argument(
            "auction epsilon must be finite and positive"
        );
    }
}

template<class Value>
[[nodiscard]] AuctionResult<Value> auction_phase(
    const std::vector<std::vector<Value>>& values,
    Value epsilon,
    std::vector<Value>& prices
){
    require_auction_epsilon(epsilon);
    const auto [bidder_count, item_count] = dimensions(values);
    if(item_count < bidder_count)[[unlikely]]{
        throw std::invalid_argument("auction requires items >= bidders");
    }
    if(bidder_count == 0){
        prices.clear();
        return {};
    }
    if(prices.empty()) prices.assign(item_count, Value{});
    if(prices.size() != item_count)[[unlikely]]{
        throw std::invalid_argument("auction price vector has wrong size");
    }
    for(const Value price : prices){
        if(!std::isfinite(price))[[unlikely]]{
            throw std::overflow_error("an auction price is not finite");
        }
    }

    AuctionResult<Value> result;
    result.bidder_to_item.assign(bidder_count, item_count);
    result.item_to_bidder.assign(item_count, bidder_count);
    std::queue<std::size_t> unassigned;
    for(std::size_t bidder = 0; bidder < bidder_count; ++bidder){
        unassigned.push(bidder);
    }
    while(!unassigned.empty()){
        const std::size_t bidder = unassigned.front();
        unassigned.pop();
        const auto net_value = [&](std::size_t item){
            const Value result_value =
                values[bidder][item] - prices[item];
            if(!std::isfinite(result_value))[[unlikely]]{
                throw std::overflow_error(
                    "an auction net value is not finite"
                );
            }
            return result_value;
        };

        std::size_t best = 0;
        std::size_t second = item_count == 1 ? 0 : 1;
        if(item_count > 1 && net_value(best) < net_value(second)){
            std::swap(best, second);
        }
        for(std::size_t item = 2; item < item_count; ++item){
            const Value candidate = net_value(item);
            if(net_value(best) < candidate){
                second = best;
                best = item;
            }else if(net_value(second) < candidate){
                second = item;
            }
        }
        const Value best_net = net_value(best);
        const Value second_net = item_count == 1
            ? best_net - epsilon
            : net_value(second);
        const Value increment = best_net - second_net + epsilon;
        Value next_price = prices[best] + increment;
        if(!(next_price > prices[best])){
            next_price = std::nextafter(
                prices[best], std::numeric_limits<Value>::infinity()
            );
        }
        if(!std::isfinite(next_price))[[unlikely]]{
            throw std::overflow_error("an auction price overflowed");
        }
        prices[best] = next_price;

        const std::size_t displaced = result.item_to_bidder[best];
        if(displaced != bidder_count){
            result.bidder_to_item[displaced] = item_count;
            unassigned.push(displaced);
        }
        result.item_to_bidder[best] = bidder;
        result.bidder_to_item[bidder] = best;
        if(result.bids == std::numeric_limits<std::size_t>::max())[[unlikely]]{
            throw std::overflow_error("auction bid count overflowed");
        }
        ++result.bids;
    }
    for(std::size_t bidder = 0; bidder < bidder_count; ++bidder){
        result.total_value +=
            values[bidder][result.bidder_to_item[bidder]];
        if(!std::isfinite(result.total_value))[[unlikely]]{
            throw std::overflow_error("auction total value overflowed");
        }
    }
    return result;
}

}  // namespace allocation_detail

template<class Value>
[[nodiscard]] AuctionResult<Value> auction_algorithm(
    const std::vector<std::vector<Value>>& values,
    Value epsilon
){
    std::vector<Value> prices;
    return allocation_detail::auction_phase(values, epsilon, prices);
}

template<class Value>
[[nodiscard]] AuctionResult<Value> epsilon_scaling_auction_algorithm(
    const std::vector<std::vector<Value>>& values,
    Value initial_epsilon,
    Value final_epsilon,
    Value scaling_factor
){
    allocation_detail::require_auction_epsilon(initial_epsilon);
    allocation_detail::require_auction_epsilon(final_epsilon);
    if(initial_epsilon < final_epsilon
       || !std::isfinite(scaling_factor)
       || !(scaling_factor > Value{1}))[[unlikely]]{
        throw std::invalid_argument("epsilon scaling parameters are invalid");
    }
    AuctionResult<Value> result;
    std::vector<Value> prices;
    std::size_t total_bids = 0;
    Value epsilon = initial_epsilon;
    while(true){
        AuctionResult<Value> phase =
            allocation_detail::auction_phase(values, epsilon, prices);
        if(phase.bids
           > std::numeric_limits<std::size_t>::max() - total_bids)[[unlikely]]{
            throw std::overflow_error("auction bid count overflowed");
        }
        total_bids += phase.bids;
        result = std::move(phase);
        if(epsilon <= final_epsilon) break;
        const Value next =
            std::max(final_epsilon, epsilon / scaling_factor);
        epsilon = next < epsilon ? next : final_epsilon;
    }
    result.bids = total_bids;
    return result;
}

struct AllocationResult {
    std::vector<std::vector<std::size_t>> bundles;
    std::vector<std::size_t> item_owner;
};

template<class Value, class URBG>
[[nodiscard]] AllocationResult random_serial_dictatorship(
    const std::vector<std::vector<Value>>& values,
    URBG& random
){
    const auto [agent_count, item_count] =
        allocation_detail::dimensions(values);
    if(agent_count == 0) return {};
    std::vector<std::size_t> order(agent_count);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::shuffle(order.begin(), order.end(), random);
    std::vector<unsigned char> available(item_count, 1);
    AllocationResult result;
    result.bundles.resize(agent_count);
    result.item_owner.assign(item_count, agent_count);
    for(const std::size_t agent : order){
        std::size_t best = item_count;
        for(std::size_t item = 0; item < item_count; ++item){
            if(!available[item]) continue;
            if(best == item_count || values[agent][best] < values[agent][item]){
                best = item;
            }
        }
        if(best == item_count) break;
        available[best] = 0;
        result.bundles[agent].push_back(best);
        result.item_owner[best] = agent;
    }
    return result;
}

template<class Value>
[[nodiscard]] AllocationResult round_robin_allocation(
    const std::vector<std::vector<Value>>& values,
    const std::vector<std::size_t>& agent_order
){
    const auto [agent_count, item_count] =
        allocation_detail::dimensions(values);
    if(agent_count == 0) return {};
    if(agent_order.empty())[[unlikely]]{
        throw std::invalid_argument("round robin order is empty");
    }
    for(const std::size_t agent : agent_order){
        if(agent >= agent_count)[[unlikely]]{
            throw std::out_of_range(
                "agent order contains invalid index"
            );
        }
    }
    std::vector<unsigned char> available(item_count, 1);
    AllocationResult result;
    result.bundles.resize(agent_count);
    result.item_owner.assign(item_count, agent_count);
    std::size_t remaining = item_count;
    for(std::size_t turn = 0; remaining != 0; ++turn){
        const std::size_t agent =
            agent_order[turn % agent_order.size()];
        std::size_t best = item_count;
        for(std::size_t item = 0; item < item_count; ++item){
            if(!available[item]) continue;
            if(best == item_count || values[agent][best] < values[agent][item]){
                best = item;
            }
        }
        available[best] = 0;
        --remaining;
        result.bundles[agent].push_back(best);
        result.item_owner[best] = agent;
    }
    return result;
}

template<class Value>
[[nodiscard]] AllocationResult envy_cycle_elimination(
    const std::vector<std::vector<Value>>& values
){
    const auto [agent_count, item_count] =
        allocation_detail::dimensions(values);
    if(agent_count == 0) return {};
    for(const auto& row : values){
        for(const Value value : row){
            if(value < Value{})[[unlikely]]{
                throw std::invalid_argument(
                    "envy-cycle elimination requires nonnegative values"
                );
            }
        }
    }

    AllocationResult result;
    result.bundles.resize(agent_count);
    result.item_owner.assign(item_count, agent_count);
    const auto bundle_value = [&](std::size_t observer, std::size_t owner){
        long double total = 0.0L;
        for(const std::size_t item : result.bundles[owner]){
            total += static_cast<long double>(values[observer][item]);
        }
        return total;
    };
    const auto envy_graph = [&]{
        std::vector<std::vector<std::size_t>> adjacency(agent_count);
        for(std::size_t observer = 0; observer < agent_count; ++observer){
            const long double own = bundle_value(observer, observer);
            for(std::size_t owner = 0; owner < agent_count; ++owner){
                if(own < bundle_value(observer, owner)){
                    adjacency[observer].push_back(owner);
                }
            }
        }
        return adjacency;
    };
    const auto find_cycle = [&](const auto& adjacency){
        std::vector<unsigned char> color(agent_count, 0);
        std::vector<std::size_t> parent(agent_count, agent_count);
        for(std::size_t root = 0; root < agent_count; ++root){
            if(color[root] != 0) continue;
            std::vector<std::pair<std::size_t, std::size_t>> stack;
            color[root] = 1;
            stack.emplace_back(root, 0);
            while(!stack.empty()){
                auto& [vertex, next_index] = stack.back();
                if(next_index == adjacency[vertex].size()){
                    color[vertex] = 2;
                    stack.pop_back();
                    continue;
                }
                const std::size_t next = adjacency[vertex][next_index++];
                if(color[next] == 0){
                    color[next] = 1;
                    parent[next] = vertex;
                    stack.emplace_back(next, 0);
                }else if(color[next] == 1){
                    std::vector<std::size_t> reverse_path;
                    for(std::size_t current = vertex;
                        current != next;
                        current = parent[current]){
                        reverse_path.push_back(current);
                    }
                    std::reverse(reverse_path.begin(), reverse_path.end());
                    std::vector<std::size_t> cycle{next};
                    cycle.insert(
                        cycle.end(),
                        reverse_path.begin(),
                        reverse_path.end()
                    );
                    return cycle;
                }
            }
        }
        return std::vector<std::size_t>{};
    };
    const auto refresh_owners = [&]{
        for(std::size_t owner = 0; owner < agent_count; ++owner){
            for(const std::size_t item : result.bundles[owner]){
                result.item_owner[item] = owner;
            }
        }
    };
    const auto eliminate_cycles = [&]{
        while(true){
            const auto adjacency = envy_graph();
            const std::vector<std::size_t> cycle =
                find_cycle(adjacency);
            if(cycle.empty()) break;
            auto first_bundle = std::move(result.bundles[cycle.front()]);
            for(std::size_t index = 0; index + 1 < cycle.size(); ++index){
                result.bundles[cycle[index]] =
                    std::move(result.bundles[cycle[index + 1]]);
            }
            result.bundles[cycle.back()] = std::move(first_bundle);
            refresh_owners();
        }
    };

    for(std::size_t item = 0; item < item_count; ++item){
        const auto adjacency = envy_graph();
        std::vector<std::size_t> indegree(agent_count, 0);
        for(const auto& row : adjacency){
            for(const std::size_t owner : row) ++indegree[owner];
        }
        std::size_t source = 0;
        while(source < agent_count && indegree[source] != 0) ++source;
        if(source == agent_count)[[unlikely]]{
            throw std::logic_error("envy graph unexpectedly contains a cycle");
        }
        result.bundles[source].push_back(item);
        result.item_owner[item] = source;
        eliminate_cycles();
    }
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_ALLOCATION_HPP_INCLUDED
