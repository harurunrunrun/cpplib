#ifndef CPPLIB_SRC_STRUCTURE_DSU_PERSISTENT_PERSISTENT_UNIONFIND_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_PERSISTENT_PERSISTENT_UNIONFIND_QUERIES_HPP_INCLUDED

#include <limits>
#include <array>
#include "../../array/static_or_vector.hpp"
#include <memory>
#include <stdexcept>

#include "persistent_dsu.hpp"

template<int MAX_SIZE = -1, int MAX_QUERY = -1>
class PersistentUnionFindQueries{
    static_assert(MAX_QUERY >= -1);

    struct State{
        PersistentDSU<MAX_SIZE, MAX_QUERY> dsu;
        cpplib_detail::StaticOrVector<int, (MAX_QUERY < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_QUERY) + 1)> version_at{std::size_t(1)};

        explicit State(int size): dsu(size){}
    };

    std::unique_ptr<State> state_;
    int processed_ = 0;

    int base_version(int base_query) const{
        if(base_query < -1 || processed_ <= base_query)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (PersistentUnionFindQueries)."
            );
        }
        return state_->version_at[static_cast<std::size_t>(base_query + 1)];
    }

    void check_capacity() const{
        if(processed_ == std::numeric_limits<int>::max() || processed_ == MAX_QUERY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: query capacity exceeded (PersistentUnionFindQueries)."
            );
        }
    }

public:
    explicit PersistentUnionFindQueries(int n = (MAX_SIZE < 0 ? 0 : MAX_SIZE)):
        state_(std::make_unique<State>(n)){}

    PersistentUnionFindQueries(const PersistentUnionFindQueries&) = delete;
    PersistentUnionFindQueries& operator=(const PersistentUnionFindQueries&) = delete;
    PersistentUnionFindQueries(PersistentUnionFindQueries&&) = default;
    PersistentUnionFindQueries& operator=(PersistentUnionFindQueries&&) = default;

    int processed_queries() const{ return processed_; }

    void merge(int base_query, int u, int v){
        check_capacity();
        const int version = state_->dsu.merge(base_version(base_query), u, v);
        state_->version_at.ensure(static_cast<std::size_t>(processed_) + 2);
        state_->version_at[static_cast<std::size_t>(processed_ + 1)] = version;
        ++processed_;
    }

    bool same(int base_query, int u, int v){
        check_capacity();
        const int version = base_version(base_query);
        const bool result = state_->dsu.same(version, u, v);
        state_->version_at.ensure(static_cast<std::size_t>(processed_) + 2);
        state_->version_at[static_cast<std::size_t>(processed_ + 1)] = version;
        ++processed_;
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_PERSISTENT_PERSISTENT_UNIONFIND_QUERIES_HPP_INCLUDED
