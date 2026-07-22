#ifndef CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_TIME_SEGMENT_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_TIME_SEGMENT_TREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

template<class Event, int MAX_TIME, int MAX_EVENTS>
class TimeSegmentTree{
    static_assert(MAX_TIME > 0);
    static_assert(MAX_EVENTS >= 0);

    static constexpr std::size_t ceil_pow2(std::size_t n){
        std::size_t result = 1;
        while(result < n) result <<= 1;
        return result;
    }

    static constexpr int tree_log(std::size_t n){
        int result = 0;
        while(std::size_t{1} << result < n) ++result;
        return result;
    }

    static constexpr std::size_t TREE_BASE = ceil_pow2(static_cast<std::size_t>(MAX_TIME));
    static constexpr std::size_t NODE_COUNT = TREE_BASE * 2;
    static constexpr int REFERENCES_PER_EVENT = tree_log(TREE_BASE) * 2 + 1;
    static constexpr std::size_t MAX_REFERENCES =
        static_cast<std::size_t>(MAX_EVENTS) * static_cast<std::size_t>(REFERENCES_PER_EVENT);

    static_assert(TREE_BASE <= static_cast<std::size_t>(std::numeric_limits<int>::max() / 2));
    static_assert(MAX_REFERENCES <= static_cast<std::size_t>(std::numeric_limits<int>::max()));

    struct Reference{
        int event;
        int next;
    };

    struct Storage{
        std::array<std::optional<Event>, static_cast<std::size_t>(MAX_EVENTS)> events;
        std::array<Reference, MAX_REFERENCES> references;
        std::array<int, NODE_COUNT> head;
        std::array<int, NODE_COUNT> tail;

        Storage(){
            head.fill(-1);
            tail.fill(-1);
        }
    };

    int time_count_ = 0;
    int event_count_ = 0;
    int reference_count_ = 0;
    std::unique_ptr<Storage> storage_;

    void check_interval(int left, int right) const{
        if(left < 0 || right < left || time_count_ < right)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (TimeSegmentTree::add_interval).");
        }
    }

    void append_reference(int node, int event){
        const int index = reference_count_++;
        storage_->references[static_cast<std::size_t>(index)] = {event, -1};
        int& head = storage_->head[static_cast<std::size_t>(node)];
        int& tail = storage_->tail[static_cast<std::size_t>(node)];
        if(head == -1){
            head = index;
        }else{
            storage_->references[static_cast<std::size_t>(tail)].next = index;
        }
        tail = index;
    }

    template<class... Args>
    void emplace_nonempty_interval(int left, int right, Args&&... args){
        std::array<int, static_cast<std::size_t>(REFERENCES_PER_EVENT)> nodes{};
        int node_count = 0;
        int l = left + static_cast<int>(TREE_BASE);
        int r = right + static_cast<int>(TREE_BASE);
        while(l < r){
            if(l & 1) nodes[static_cast<std::size_t>(node_count++)] = l++;
            if(r & 1) nodes[static_cast<std::size_t>(node_count++)] = --r;
            l >>= 1;
            r >>= 1;
        }
        if(event_count_ == MAX_EVENTS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (TimeSegmentTree::add_interval event).");
        }
        if(static_cast<std::size_t>(reference_count_) + static_cast<std::size_t>(node_count) > MAX_REFERENCES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (TimeSegmentTree::add_interval reference).");
        }

        const int event = event_count_;
        storage_->events[static_cast<std::size_t>(event)].emplace(std::forward<Args>(args)...);
        ++event_count_;
        for(int i = 0; i < node_count; ++i){
            append_reference(nodes[static_cast<std::size_t>(i)], event);
        }
    }

    template<class Apply, class Snapshot, class Rollback, class Answer>
    void dfs(
        int node,
        int left,
        int right,
        Apply& apply,
        Snapshot& snapshot,
        Rollback& rollback,
        Answer& answer
    ) const{
        if(time_count_ <= left) return;
        const int first_reference = storage_->head[static_cast<std::size_t>(node)];
        if(first_reference == -1){
            if(right - left == 1){
                std::invoke(answer, left);
            }else{
                const int middle = (left + right) >> 1;
                dfs(node << 1, left, middle, apply, snapshot, rollback, answer);
                dfs(node << 1 | 1, middle, right, apply, snapshot, rollback, answer);
            }
            return;
        }

        auto checkpoint = std::invoke(snapshot);
        for(int ref = first_reference; ref != -1;){
            const Reference& entry = storage_->references[static_cast<std::size_t>(ref)];
            std::invoke(apply, *storage_->events[static_cast<std::size_t>(entry.event)]);
            ref = entry.next;
        }
        if(right - left == 1){
            std::invoke(answer, left);
        }else{
            const int middle = (left + right) >> 1;
            dfs(node << 1, left, middle, apply, snapshot, rollback, answer);
            dfs(node << 1 | 1, middle, right, apply, snapshot, rollback, answer);
        }
        std::invoke(rollback, std::move(checkpoint));
    }

public:
    explicit TimeSegmentTree(int time_count = MAX_TIME):
        time_count_(time_count){
        if(time_count < 0 || MAX_TIME < time_count)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (TimeSegmentTree::TimeSegmentTree).");
        }
        storage_ = std::make_unique<Storage>();
    }

    TimeSegmentTree(const TimeSegmentTree&) = delete;
    TimeSegmentTree& operator=(const TimeSegmentTree&) = delete;
    TimeSegmentTree(TimeSegmentTree&&) = default;
    TimeSegmentTree& operator=(TimeSegmentTree&&) = default;

    int size() const{ return time_count_; }
    int event_count() const{ return event_count_; }
    int reference_count() const{ return reference_count_; }
    static constexpr int event_capacity(){ return MAX_EVENTS; }
    static constexpr int time_capacity(){ return MAX_TIME; }
    static constexpr std::size_t reference_capacity(){ return MAX_REFERENCES; }

    void add_interval(int left, int right, const Event& event){
        check_interval(left, right);
        if(left == right) return;
        emplace_nonempty_interval(left, right, event);
    }

    void add_interval(int left, int right, Event&& event){
        check_interval(left, right);
        if(left == right) return;
        emplace_nonempty_interval(left, right, std::move(event));
    }

    template<class... Args>
    void emplace_interval(int left, int right, Args&&... args){
        check_interval(left, right);
        if(left == right) return;
        emplace_nonempty_interval(left, right, std::forward<Args>(args)...);
    }

    void clear(){
        for(int i = 0; i < event_count_; ++i){
            storage_->events[static_cast<std::size_t>(i)].reset();
        }
        storage_->head.fill(-1);
        storage_->tail.fill(-1);
        event_count_ = 0;
        reference_count_ = 0;
    }

    template<class Apply, class Snapshot, class Rollback, class Answer>
    void run(Apply&& apply, Snapshot&& snapshot, Rollback&& rollback, Answer&& answer) const{
        if(time_count_ == 0) return;
        dfs(
            1,
            0,
            static_cast<int>(TREE_BASE),
            apply,
            snapshot,
            rollback,
            answer
        );
    }
};

template<class Event, int MAX_TIME, int MAX_EVENTS>
using OfflineIntervalEvents = TimeSegmentTree<Event, MAX_TIME, MAX_EVENTS>;

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_TIME_SEGMENT_TREE_HPP_INCLUDED
