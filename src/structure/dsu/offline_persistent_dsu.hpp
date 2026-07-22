#ifndef CPPLIB_SRC_STRUCTURE_DSU_OFFLINE_PERSISTENT_DSU_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_OFFLINE_PERSISTENT_DSU_HPP_INCLUDED

#include <cstddef>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../array/rollback_array.hpp"

template<int MAX_SIZE, int MAX_VERSION, int MAX_QUERY = MAX_VERSION>
class OfflinePersistentDSU{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_VERSION >= 0);
    static_assert(MAX_QUERY >= 0);
    static_assert(MAX_VERSION <= std::numeric_limits<int>::max() / 2);
    static_assert(MAX_QUERY < std::numeric_limits<int>::max());

    static int checked_size(int size){
        if(size < 0 || MAX_SIZE < size)[[unlikely]]{
            throw std::out_of_range("OfflinePersistentDSU size is out of range");
        }
        return size;
    }

    struct VersionEdge{
        int child;
        int left;
        int right;
    };
    struct Query{
        int index;
        int left;
        int right;
    };
    struct State{
        RollbackArray<int, MAX_SIZE, 2 * MAX_VERSION> parent_or_size;

        explicit State(int size): parent_or_size(size, -1){}
    };
    struct Frame{
        int version;
        std::size_t next_child;
        typename RollbackArray<int, MAX_SIZE, 2 * MAX_VERSION>::Snapshot snapshot;
        bool entered;
    };

    int size_ = 0;
    int version_count_ = 1;
    int query_count_ = 0;
    std::unique_ptr<State> state_;
    std::vector<std::vector<VersionEdge>> children_;
    std::vector<std::vector<Query>> queries_;

    void check_vertex(int vertex, const char* operation) const{
        if(vertex < 0 || size_ <= vertex)[[unlikely]]{
            throw std::out_of_range(operation);
        }
    }
    void check_version(int version, const char* operation) const{
        if(version < 0 || version_count_ <= version)[[unlikely]]{
            throw std::out_of_range(operation);
        }
    }
    int leader_unchecked(int vertex) const{
        while(state_->parent_or_size.get(vertex) >= 0){
            vertex = state_->parent_or_size.get(vertex);
        }
        return vertex;
    }
    void merge_unchecked(int left, int right){
        left = leader_unchecked(left);
        right = leader_unchecked(right);
        if(left == right) return;
        if(state_->parent_or_size.get(left) >
           state_->parent_or_size.get(right)){
            std::swap(left, right);
        }
        state_->parent_or_size.set(
            left,
            state_->parent_or_size.get(left) +
                state_->parent_or_size.get(right)
        );
        state_->parent_or_size.set(right, left);
    }
    int append_version(int parent, int left, int right){
        check_version(parent, "OfflinePersistentDSU version is out of range");
        if(version_count_ > MAX_VERSION)[[unlikely]]{
            throw std::length_error("OfflinePersistentDSU version capacity exceeded");
        }
        const int created = version_count_;
        children_[static_cast<std::size_t>(parent)].push_back({
            created, left, right
        });
        ++version_count_;
        return created;
    }

public:
    explicit OfflinePersistentDSU(int size = MAX_SIZE):
        size_(checked_size(size)),
        state_(std::make_unique<State>(size_)),
        children_(static_cast<std::size_t>(MAX_VERSION + 1)),
        queries_(static_cast<std::size_t>(MAX_VERSION + 1)){}

    OfflinePersistentDSU(const OfflinePersistentDSU&) = delete;
    OfflinePersistentDSU& operator=(const OfflinePersistentDSU&) = delete;
    OfflinePersistentDSU(OfflinePersistentDSU&&) noexcept = default;
    OfflinePersistentDSU& operator=(OfflinePersistentDSU&&) noexcept = default;

    int size() const noexcept{ return size_; }
    int versions() const noexcept{ return version_count_; }
    int query_count() const noexcept{ return query_count_; }

    int merge(int version, int left, int right){
        check_vertex(left, "OfflinePersistentDSU merge vertex is out of range");
        check_vertex(right, "OfflinePersistentDSU merge vertex is out of range");
        return append_version(version, left, right);
    }

    int fork(int version){
        return append_version(version, -1, -1);
    }

    int query_same(int version, int left, int right){
        check_version(version, "OfflinePersistentDSU query version is out of range");
        check_vertex(left, "OfflinePersistentDSU query vertex is out of range");
        check_vertex(right, "OfflinePersistentDSU query vertex is out of range");
        if(query_count_ == MAX_QUERY)[[unlikely]]{
            throw std::length_error("OfflinePersistentDSU query capacity exceeded");
        }
        const int index = query_count_;
        queries_[static_cast<std::size_t>(version)].push_back({
            index, left, right
        });
        ++query_count_;
        return index;
    }

    std::vector<int> solve(){
        std::vector<int> answer(static_cast<std::size_t>(query_count_), -1);
        std::vector<Frame> stack;
        stack.reserve(static_cast<std::size_t>(version_count_));
        stack.push_back({0, 0, state_->parent_or_size.snapshot(), false});
        while(!stack.empty()){
            Frame& frame = stack.back();
            if(!frame.entered){
                for(const Query& query:
                    queries_[static_cast<std::size_t>(frame.version)]){
                    answer[static_cast<std::size_t>(query.index)] =
                        leader_unchecked(query.left) ==
                        leader_unchecked(query.right);
                }
                frame.entered = true;
            }
            auto& children = children_[static_cast<std::size_t>(frame.version)];
            if(frame.next_child < children.size()){
                const VersionEdge edge = children[frame.next_child++];
                const auto snapshot = state_->parent_or_size.snapshot();
                if(edge.left != -1) merge_unchecked(edge.left, edge.right);
                stack.push_back({edge.child, 0, snapshot, false});
            }else{
                const auto snapshot = frame.snapshot;
                stack.pop_back();
                state_->parent_or_size.rollback(snapshot);
            }
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_OFFLINE_PERSISTENT_DSU_HPP_INCLUDED
