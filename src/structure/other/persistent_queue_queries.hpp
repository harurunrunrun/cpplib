#pragma once

#include <memory>
#include <stdexcept>

template<class Queue, int MAX_QUERY>
class PersistentQueueQueries{
    static_assert(MAX_QUERY >= 0);

    std::unique_ptr<Queue> queue_;
    int processed_ = 0;

    int base_version(int base_query) const{
        if(base_query < -1 || processed_ <= base_query)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (PersistentQueueQueries)."
            );
        }
        return base_query + 1;
    }

    void check_capacity() const{
        if(processed_ == MAX_QUERY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: query capacity exceeded (PersistentQueueQueries)."
            );
        }
    }

public:
    using value_type = typename Queue::value_type;

    explicit PersistentQueueQueries(int capacity):
        queue_(std::make_unique<Queue>(capacity)){}

    PersistentQueueQueries(const PersistentQueueQueries&) = delete;
    PersistentQueueQueries& operator=(const PersistentQueueQueries&) = delete;
    PersistentQueueQueries(PersistentQueueQueries&&) = default;
    PersistentQueueQueries& operator=(PersistentQueueQueries&&) = default;

    int processed_queries() const{ return processed_; }

    void push(int base_query, const value_type& value){
        check_capacity();
        (void)queue_->push(base_version(base_query), value);
        ++processed_;
    }

    value_type pop(int base_query){
        check_capacity();
        const int version = base_version(base_query);
        const value_type result = queue_->front(version);
        (void)queue_->pop(version);
        ++processed_;
        return result;
    }
};
