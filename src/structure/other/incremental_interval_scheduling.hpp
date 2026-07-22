#ifndef CPPLIB_SRC_STRUCTURE_OTHER_INCREMENTAL_INTERVAL_SCHEDULING_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_INCREMENTAL_INTERVAL_SCHEDULING_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<class Coordinate>
class IncrementalIntervalScheduling{
    static_assert(
        std::is_integral_v<Coordinate> && !std::is_same_v<std::remove_cv_t<Coordinate>, bool>,
        "Coordinate must be an integral type other than bool"
    );

public:
    struct Interval{
        Coordinate start;
        Coordinate finish;
        std::size_t id;
    };

    struct InsertionResult{
        std::size_t id;
        bool active;
        std::size_t deactivated_count;
        std::size_t maximum_count;
    };

private:
    class TransitionForest{
        struct Node{
            int child[2] = {-1, -1};
            int parent = -1;
            std::size_t sum = 0;
            unsigned char value = 0;
        };

        std::vector<Node> nodes_;

        [[nodiscard]] bool is_auxiliary_root(int vertex) const noexcept{
            const int parent = nodes_[static_cast<std::size_t>(vertex)].parent;
            return parent == -1
                || (nodes_[static_cast<std::size_t>(parent)].child[0] != vertex
                    && nodes_[static_cast<std::size_t>(parent)].child[1] != vertex);
        }

        [[nodiscard]] std::size_t sum(int vertex) const noexcept{
            return vertex == -1 ? 0 : nodes_[static_cast<std::size_t>(vertex)].sum;
        }

        void pull(int vertex) noexcept{
            auto& node = nodes_[static_cast<std::size_t>(vertex)];
            node.sum = sum(node.child[0]) + static_cast<std::size_t>(node.value)
                + sum(node.child[1]);
        }

        void rotate(int vertex) noexcept{
            auto& node = nodes_[static_cast<std::size_t>(vertex)];
            const int parent = node.parent;
            auto& parent_node = nodes_[static_cast<std::size_t>(parent)];
            const int grandparent = parent_node.parent;
            const int direction = parent_node.child[1] == vertex ? 1 : 0;
            const int middle = node.child[direction ^ 1];

            if(!is_auxiliary_root(parent)){
                auto& grandparent_node = nodes_[static_cast<std::size_t>(grandparent)];
                grandparent_node.child[grandparent_node.child[1] == parent ? 1 : 0] = vertex;
            }
            node.parent = grandparent;
            node.child[direction ^ 1] = parent;
            parent_node.parent = vertex;
            parent_node.child[direction] = middle;
            if(middle != -1) nodes_[static_cast<std::size_t>(middle)].parent = parent;
            pull(parent);
            pull(vertex);
        }

        void splay(int vertex) noexcept{
            while(!is_auxiliary_root(vertex)){
                const int parent = nodes_[static_cast<std::size_t>(vertex)].parent;
                if(!is_auxiliary_root(parent)){
                    const int grandparent = nodes_[static_cast<std::size_t>(parent)].parent;
                    const bool vertex_is_right =
                        nodes_[static_cast<std::size_t>(parent)].child[1] == vertex;
                    const bool parent_is_right =
                        nodes_[static_cast<std::size_t>(grandparent)].child[1] == parent;
                    rotate(vertex_is_right == parent_is_right ? parent : vertex);
                }
                rotate(vertex);
            }
        }

        void access(int vertex) noexcept{
            int preferred_child = -1;
            for(int current = vertex; current != -1;){
                splay(current);
                auto& node = nodes_[static_cast<std::size_t>(current)];
                const int path_parent = node.parent;
                node.child[1] = preferred_child;
                if(preferred_child != -1){
                    nodes_[static_cast<std::size_t>(preferred_child)].parent = current;
                }
                pull(current);
                preferred_child = current;
                current = path_parent;
            }
            splay(vertex);
        }

    public:
        explicit TransitionForest(std::size_t node_count): nodes_(node_count){
            for(std::size_t index = 0; index + 1 < node_count; ++index){
                nodes_[index].parent = static_cast<int>(index + 1);
            }
        }

        void set_transition(std::size_t source, std::size_t destination, bool takes_interval) noexcept{
            const int source_vertex = static_cast<int>(source);
            access(source_vertex);
            auto& source_node = nodes_[source];
            const int ancestors = source_node.child[0];
            source_node.child[0] = -1;
            if(ancestors != -1) nodes_[static_cast<std::size_t>(ancestors)].parent = -1;
            source_node.value = static_cast<unsigned char>(takes_interval);
            pull(source_vertex);
            source_node.parent = static_cast<int>(destination);
        }

        [[nodiscard]] std::size_t path_sum(std::size_t source) noexcept{
            const int source_vertex = static_cast<int>(source);
            access(source_vertex);
            return nodes_[source].sum;
        }
    };

    struct ActiveInterval{
        Coordinate finish;
        std::size_t id;
    };

    using UnsignedCoordinate = std::make_unsigned_t<Coordinate>;
    using ActiveMap = std::map<Coordinate, ActiveInterval>;

    Coordinate minimum_coordinate_;
    Coordinate maximum_coordinate_;
    TransitionForest transitions_;
    std::vector<Interval> intervals_;
    std::vector<unsigned char> active_flags_;
    ActiveMap active_intervals_;
    std::size_t maximum_count_ = 0;

    [[nodiscard]] static UnsignedCoordinate distance(
        Coordinate lower,
        Coordinate upper
    ) noexcept{
        return static_cast<UnsignedCoordinate>(upper)
            - static_cast<UnsignedCoordinate>(lower);
    }

    [[nodiscard]] static std::size_t checked_node_count(
        Coordinate minimum_coordinate,
        Coordinate maximum_coordinate
    ){
        if(!(minimum_coordinate < maximum_coordinate)){
            throw std::invalid_argument(
                "IncrementalIntervalScheduling requires minimum_coordinate < maximum_coordinate"
            );
        }
        const UnsignedCoordinate span = distance(minimum_coordinate, maximum_coordinate);
        const auto maximum_index = static_cast<std::uintmax_t>(
            std::numeric_limits<int>::max() - 1
        );
        if(static_cast<std::uintmax_t>(span) > maximum_index){
            throw std::length_error("coordinate range is too large");
        }
        return static_cast<std::size_t>(span) + 1;
    }

    [[nodiscard]] std::size_t index_of(Coordinate coordinate) const noexcept{
        return static_cast<std::size_t>(distance(minimum_coordinate_, coordinate));
    }

    void make_empty_transition(Coordinate start) noexcept{
        const std::size_t source = index_of(start);
        transitions_.set_transition(source, source + 1, false);
    }

    void make_interval_transition(Coordinate start, Coordinate finish) noexcept{
        transitions_.set_transition(index_of(start), index_of(finish), true);
    }

public:
    IncrementalIntervalScheduling(
        Coordinate minimum_coordinate,
        Coordinate maximum_coordinate
    ):
        minimum_coordinate_(minimum_coordinate),
        maximum_coordinate_(maximum_coordinate),
        transitions_(checked_node_count(minimum_coordinate, maximum_coordinate)){}

    [[nodiscard]] Coordinate minimum_coordinate() const noexcept{
        return minimum_coordinate_;
    }

    [[nodiscard]] Coordinate maximum_coordinate() const noexcept{
        return maximum_coordinate_;
    }

    [[nodiscard]] std::size_t interval_count() const noexcept{
        return intervals_.size();
    }

    [[nodiscard]] std::size_t active_interval_count() const noexcept{
        return active_intervals_.size();
    }

    [[nodiscard]] std::size_t maximum_count() const noexcept{
        return maximum_count_;
    }

    void reserve_intervals(std::size_t count){
        intervals_.reserve(count);
        active_flags_.reserve(count);
    }

    InsertionResult insert(Coordinate start, Coordinate finish){
        if(!(start < finish)){
            throw std::invalid_argument(
                "IncrementalIntervalScheduling requires start < finish"
            );
        }
        if(start < minimum_coordinate_ || maximum_coordinate_ < finish){
            throw std::out_of_range("interval is outside the configured coordinate range");
        }

        const std::size_t id = intervals_.size();
        intervals_.push_back({start, finish, id});
        try{
            active_flags_.push_back(0);
        }catch(...){
            intervals_.pop_back();
            throw;
        }

        const auto first_not_before = active_intervals_.lower_bound(start);
        if(first_not_before != active_intervals_.end()
            && !(finish < first_not_before->second.finish)){
            return {id, false, 0, maximum_count_};
        }

        ActiveMap staged;
        try{
            staged.emplace(start, ActiveInterval{finish, id});
        }catch(...){
            active_flags_.pop_back();
            intervals_.pop_back();
            throw;
        }
        auto staged_node = staged.extract(staged.begin());

        std::size_t deactivated_count = 0;
        auto position = active_intervals_.upper_bound(start);
        while(position != active_intervals_.begin()){
            const auto previous = std::prev(position);
            if(previous->second.finish < finish) break;
            active_flags_[previous->second.id] = 0;
            make_empty_transition(previous->first);
            position = active_intervals_.erase(previous);
            ++deactivated_count;
        }

        const auto inserted = active_intervals_.insert(std::move(staged_node));
        active_flags_[id] = 1;
        make_interval_transition(inserted.position->first, finish);
        maximum_count_ = transitions_.path_sum(0);
        return {id, true, deactivated_count, maximum_count_};
    }

    [[nodiscard]] bool is_active(std::size_t id) const{
        if(id >= intervals_.size()){
            throw std::out_of_range("interval id is out of range");
        }
        return active_flags_[id] != 0;
    }

    [[nodiscard]] const Interval& interval(std::size_t id) const{
        if(id >= intervals_.size()){
            throw std::out_of_range("interval id is out of range");
        }
        return intervals_[id];
    }

    [[nodiscard]] std::vector<std::size_t> optimal_schedule_ids() const{
        std::vector<std::size_t> result;
        result.reserve(maximum_count_);
        auto iterator = active_intervals_.lower_bound(minimum_coordinate_);
        while(iterator != active_intervals_.end()){
            result.push_back(iterator->second.id);
            iterator = active_intervals_.lower_bound(iterator->second.finish);
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_INCREMENTAL_INTERVAL_SCHEDULING_HPP_INCLUDED
