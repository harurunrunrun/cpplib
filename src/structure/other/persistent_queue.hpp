#ifndef CPPLIB_SRC_STRUCTURE_OTHER_PERSISTENT_QUEUE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_PERSISTENT_QUEUE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "../array/persistent_array.hpp"
#include "../segtree/persistent_segtree.hpp"

template<class PersistentStorage, int MAX_VERSION>
struct PersistentQueue{
    static_assert(MAX_VERSION >= 0);
    static_assert(MAX_VERSION < std::numeric_limits<int>::max());

    using value_type = std::remove_cvref_t<decltype(
        std::declval<const PersistentStorage&>().get(0, 0)
    )>;

private:
    struct VersionState{
        int storage_version = 0;
        int front_index = 0;
        int element_count = 0;
    };

    PersistentStorage storage_;
    std::array<VersionState, static_cast<std::size_t>(MAX_VERSION) + 1>
        state_{};
    int version_count_ = 1;

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count_ <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_new_version(const char* message) const{
        if(version_count_ > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int append_version(const VersionState& next){
        const int result = version_count_++;
        state_[static_cast<std::size_t>(result)] = next;
        return result;
    }

public:
    PersistentQueue(const PersistentQueue&) = delete;
    PersistentQueue& operator=(const PersistentQueue&) = delete;
    PersistentQueue(PersistentQueue&&) = delete;
    PersistentQueue& operator=(PersistentQueue&&) = delete;

    explicit PersistentQueue(int capacity) : storage_(capacity) {}

    int capacity() const{
        return storage_.size();
    }

    int versions() const{
        return version_count_;
    }

    int latest_version() const{
        return version_count_ - 1;
    }

    int size(int version) const{
        check_version(
            version,
            "library assertion fault: range violation (PersistentQueue::size)."
        );
        return state_[static_cast<std::size_t>(version)].element_count;
    }

    bool empty(int version) const{
        return size(version) == 0;
    }

    value_type front(int version) const{
        check_version(
            version,
            "library assertion fault: range violation (PersistentQueue::front)."
        );
        const VersionState& current = state_[static_cast<std::size_t>(version)];
        if(current.element_count == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty queue (PersistentQueue::front)."
            );
        }
        return storage_.get(current.storage_version, current.front_index);
    }

    int push(int version, const value_type& value){
        check_version(
            version,
            "library assertion fault: range violation (PersistentQueue::push)."
        );
        check_new_version(
            "library assertion fault: version capacity exceeded (PersistentQueue::push)."
        );

        const VersionState& current = state_[static_cast<std::size_t>(version)];
        const int queue_capacity = capacity();
        if(current.element_count == queue_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: queue capacity exceeded (PersistentQueue::push)."
            );
        }

        const int write_index = static_cast<int>(
            (static_cast<long long>(current.front_index) + current.element_count)
            % queue_capacity
        );
        const int storage_version = storage_.set(
            current.storage_version, write_index, value
        );
        return append_version({
            storage_version,
            current.front_index,
            current.element_count + 1
        });
    }

    int pop(int version){
        check_version(
            version,
            "library assertion fault: range violation (PersistentQueue::pop)."
        );
        check_new_version(
            "library assertion fault: version capacity exceeded (PersistentQueue::pop)."
        );

        const VersionState& current = state_[static_cast<std::size_t>(version)];
        if(current.element_count == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty queue (PersistentQueue::pop)."
            );
        }

        int next_front = current.front_index + 1;
        if(next_front == capacity()) next_front = 0;
        return append_version({
            current.storage_version,
            next_front,
            current.element_count - 1
        });
    }
};

template<class T, int MAX_SIZE, int MAX_VERSION>
using PersistentArrayQueue = PersistentQueue<
    PersistentArray<T, MAX_SIZE, MAX_VERSION>,
    MAX_VERSION
>;

template<auto Monoid, int MAX_SIZE, int MAX_VERSION>
using PersistentSegtreeQueue = PersistentQueue<
    PersistentSegtree<Monoid, MAX_SIZE, MAX_VERSION>,
    MAX_VERSION
>;

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_PERSISTENT_QUEUE_HPP_INCLUDED
