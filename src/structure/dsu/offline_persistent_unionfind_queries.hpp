#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#include "offline_persistent_dsu.hpp"

template<int MAX_SIZE, int MAX_OPERATION, int MAX_QUERY = MAX_OPERATION>
class OfflinePersistentUnionFindQueries{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_OPERATION >= 0);
    static_assert(MAX_QUERY >= 0);

    struct State{
        std::array<int, static_cast<std::size_t>(MAX_OPERATION) + 1>
            version_at{};
    };

    OfflinePersistentDSU<MAX_SIZE, MAX_OPERATION, MAX_QUERY> dsu_;
    std::unique_ptr<State> state_;
    int operation_count_ = 0;

    void check_capacity() const{
        if(operation_count_ == MAX_OPERATION)[[unlikely]]{
            throw std::length_error(
                "OfflinePersistentUnionFindQueries operation capacity exceeded"
            );
        }
    }

    int base_version(int base_operation) const{
        if(base_operation < -1 || operation_count_ <= base_operation)[[unlikely]]{
            throw std::out_of_range(
                "OfflinePersistentUnionFindQueries base operation is out of range"
            );
        }
        return state_->version_at[static_cast<std::size_t>(base_operation + 1)];
    }

    void append_operation(int version){
        state_->version_at[static_cast<std::size_t>(operation_count_ + 1)] =
            version;
        ++operation_count_;
    }

public:
    explicit OfflinePersistentUnionFindQueries(int size = MAX_SIZE):
        dsu_(size), state_(std::make_unique<State>()){}

    OfflinePersistentUnionFindQueries(
        const OfflinePersistentUnionFindQueries&
    ) = delete;
    OfflinePersistentUnionFindQueries& operator=(
        const OfflinePersistentUnionFindQueries&
    ) = delete;
    OfflinePersistentUnionFindQueries(
        OfflinePersistentUnionFindQueries&&
    ) noexcept = default;
    OfflinePersistentUnionFindQueries& operator=(
        OfflinePersistentUnionFindQueries&&
    ) noexcept = default;

    int size() const noexcept{ return dsu_.size(); }
    int operations() const noexcept{ return operation_count_; }
    int query_count() const noexcept{ return dsu_.query_count(); }

    void merge(int base_operation, int left, int right){
        check_capacity();
        const int version = dsu_.merge(
            base_version(base_operation), left, right
        );
        append_operation(version);
    }

    int same(int base_operation, int left, int right){
        check_capacity();
        const int version = base_version(base_operation);
        const int query = dsu_.query_same(version, left, right);
        append_operation(version);
        return query;
    }

    std::vector<int> solve(){ return dsu_.solve(); }
};
