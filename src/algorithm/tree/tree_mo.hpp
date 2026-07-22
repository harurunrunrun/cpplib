#ifndef CPPLIB_SRC_ALGORITHM_TREE_TREE_MO_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_TREE_MO_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

template<int MAX_SIZE>
class TreeMo{
    static_assert(MAX_SIZE >= 0);

    static constexpr int EDGE_CAPACITY =
        MAX_SIZE == 0 ? 0 : 2 * (MAX_SIZE - 1);
    static constexpr int MAX_LOG = static_cast<int>(
        std::bit_width(static_cast<unsigned int>(MAX_SIZE == 0 ? 1 : MAX_SIZE))
    );

    struct Query{
        int left;
        int right;
        int extra;
        int index;
    };

    struct State{
        std::array<int, MAX_SIZE> head{};
        std::array<int, EDGE_CAPACITY> to{};
        std::array<int, EDGE_CAPACITY> next{};
        std::array<int, MAX_SIZE> parent{};
        std::array<int, MAX_SIZE> depth{};
        std::array<int, MAX_SIZE> tin{};
        std::array<int, MAX_SIZE> tout{};
        std::array<int, 2 * MAX_SIZE> euler{};
        std::array<int, MAX_LOG * MAX_SIZE> up{};
        std::array<int, MAX_SIZE> stack_vertex{};
        std::array<int, MAX_SIZE> stack_edge{};
    };

    int _n;
    int directed_edge_count = 0;
    int _root = -1;
    bool built = false;
    std::unique_ptr<State> state;
    std::vector<Query> queries;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void require_built(const char* message) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int table(int level, int vertex) const{
        return state->up[static_cast<std::size_t>(level * MAX_SIZE + vertex)];
    }

public:
    explicit TreeMo(int n = MAX_SIZE): _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        state = std::make_unique<State>();
        state->head.fill(-1);
        queries.reserve(static_cast<std::size_t>(n));
    }

    TreeMo(const TreeMo&) = delete;
    TreeMo& operator=(const TreeMo&) = delete;
    TreeMo(TreeMo&&) = default;
    TreeMo& operator=(TreeMo&&) = default;

    int size() const{
        return _n;
    }

    int edge_count() const{
        return directed_edge_count / 2;
    }

    int query_count() const{
        return static_cast<int>(queries.size());
    }

    void reserve_queries(int count){
        if(count < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (reserve_queries)."
            );
        }
        queries.reserve(static_cast<std::size_t>(count));
    }

    void add_edge(int left, int right){
        if(built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge added after build (add_edge)."
            );
        }
        check_vertex(
            left,
            "library assertion fault: range violation (add_edge)."
        );
        check_vertex(
            right,
            "library assertion fault: range violation (add_edge)."
        );
        if(directed_edge_count + 2 > EDGE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge capacity exceeded (add_edge)."
            );
        }
        state->to[static_cast<std::size_t>(directed_edge_count)] = right;
        state->next[static_cast<std::size_t>(directed_edge_count)] =
            state->head[static_cast<std::size_t>(left)];
        state->head[static_cast<std::size_t>(left)] = directed_edge_count++;

        state->to[static_cast<std::size_t>(directed_edge_count)] = left;
        state->next[static_cast<std::size_t>(directed_edge_count)] =
            state->head[static_cast<std::size_t>(right)];
        state->head[static_cast<std::size_t>(right)] = directed_edge_count++;
    }

    void build(int root = 0){
        if(!queries.empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: queries exist (build)."
            );
        }
        built = false;
        if(_n == 0){
            if(directed_edge_count != 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (build)."
                );
            }
            _root = -1;
            built = true;
            return;
        }
        check_vertex(
            root,
            "library assertion fault: range violation (build)."
        );
        if(directed_edge_count != 2 * (_n - 1))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not a tree (build)."
            );
        }

        std::fill_n(state->parent.begin(), _n, -2);
        std::fill_n(state->depth.begin(), _n, 0);
        std::fill_n(state->tin.begin(), _n, -1);
        std::fill_n(state->tout.begin(), _n, -1);

        _root = root;
        state->parent[static_cast<std::size_t>(root)] = -1;
        int timer = 0;
        int visited = 1;
        state->tin[static_cast<std::size_t>(root)] = timer;
        state->euler[static_cast<std::size_t>(timer++)] = root;
        state->stack_vertex[0] = root;
        state->stack_edge[0] = state->head[static_cast<std::size_t>(root)];
        int stack_size = 1;

        while(stack_size != 0){
            const int top = stack_size - 1;
            const int vertex = state->stack_vertex[static_cast<std::size_t>(top)];
            int& edge = state->stack_edge[static_cast<std::size_t>(top)];
            if(edge == -1){
                state->tout[static_cast<std::size_t>(vertex)] = timer;
                state->euler[static_cast<std::size_t>(timer++)] = vertex;
                --stack_size;
                continue;
            }

            const int current_edge = edge;
            edge = state->next[static_cast<std::size_t>(edge)];
            const int to = state->to[static_cast<std::size_t>(current_edge)];
            if(to == state->parent[static_cast<std::size_t>(vertex)]){
                continue;
            }
            if(state->parent[static_cast<std::size_t>(to)] != -2)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (build)."
                );
            }
            state->parent[static_cast<std::size_t>(to)] = vertex;
            state->depth[static_cast<std::size_t>(to)] =
                state->depth[static_cast<std::size_t>(vertex)] + 1;
            state->tin[static_cast<std::size_t>(to)] = timer;
            state->euler[static_cast<std::size_t>(timer++)] = to;
            state->stack_vertex[static_cast<std::size_t>(stack_size)] = to;
            state->stack_edge[static_cast<std::size_t>(stack_size)] =
                state->head[static_cast<std::size_t>(to)];
            ++stack_size;
            ++visited;
        }

        if(visited != _n || timer != 2 * _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not connected (build)."
            );
        }

        for(int vertex = 0; vertex < _n; ++vertex){
            state->up[static_cast<std::size_t>(vertex)] =
                state->parent[static_cast<std::size_t>(vertex)] == -1
                    ? vertex
                    : state->parent[static_cast<std::size_t>(vertex)];
        }
        for(int level = 1; level < MAX_LOG; ++level){
            const int base = level * MAX_SIZE;
            const int previous = (level - 1) * MAX_SIZE;
            for(int vertex = 0; vertex < _n; ++vertex){
                const int middle = state->up[
                    static_cast<std::size_t>(previous + vertex)
                ];
                state->up[static_cast<std::size_t>(base + vertex)] =
                    state->up[static_cast<std::size_t>(previous + middle)];
            }
        }
        built = true;
    }

    int root() const{
        require_built(
            "library assertion fault: TreeMo is not built (root)."
        );
        return _root;
    }

    int parent(int vertex) const{
        require_built(
            "library assertion fault: TreeMo is not built (parent)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (parent)."
        );
        return state->parent[static_cast<std::size_t>(vertex)];
    }

    int depth(int vertex) const{
        require_built(
            "library assertion fault: TreeMo is not built (depth)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (depth)."
        );
        return state->depth[static_cast<std::size_t>(vertex)];
    }

    int lca(int left, int right) const{
        require_built(
            "library assertion fault: TreeMo is not built (lca)."
        );
        check_vertex(
            left,
            "library assertion fault: range violation (lca)."
        );
        check_vertex(
            right,
            "library assertion fault: range violation (lca)."
        );
        if(state->depth[static_cast<std::size_t>(left)]
            < state->depth[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        int difference = state->depth[static_cast<std::size_t>(left)]
            - state->depth[static_cast<std::size_t>(right)];
        for(int level = 0; difference != 0; ++level){
            if(difference & 1){
                left = table(level, left);
            }
            difference >>= 1;
        }
        if(left == right){
            return left;
        }
        for(int level = MAX_LOG - 1; level >= 0; --level){
            if(table(level, left) != table(level, right)){
                left = table(level, left);
                right = table(level, right);
            }
        }
        return state->parent[static_cast<std::size_t>(left)];
    }

    int add_query(int left, int right){
        require_built(
            "library assertion fault: TreeMo is not built (add_query)."
        );
        check_vertex(
            left,
            "library assertion fault: range violation (add_query)."
        );
        check_vertex(
            right,
            "library assertion fault: range violation (add_query)."
        );
        if(state->tin[static_cast<std::size_t>(left)]
            > state->tin[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        const int ancestor = lca(left, right);
        const int index = static_cast<int>(queries.size());
        if(ancestor == left){
            queries.push_back({
                state->tin[static_cast<std::size_t>(left)],
                state->tin[static_cast<std::size_t>(right)] + 1,
                -1,
                index
            });
        }else{
            queries.push_back({
                state->tout[static_cast<std::size_t>(left)],
                state->tin[static_cast<std::size_t>(right)] + 1,
                ancestor,
                index
            });
        }
        return index;
    }

    void clear_queries(){
        queries.clear();
    }

    template<class Add, class Erase, class Answer>
    void solve(
        Add add,
        Erase erase,
        Answer answer,
        int block_size = -1
    ) const{
        require_built(
            "library assertion fault: TreeMo is not built (solve)."
        );
        if(queries.empty()){
            return;
        }
        if(block_size <= 0){
            block_size = std::max(
                1,
                static_cast<int>(
                    static_cast<long double>(2 * _n)
                    / std::sqrt(static_cast<long double>(queries.size()))
                )
            );
        }

        std::vector<Query> order = queries;
        std::sort(order.begin(), order.end(), [&](const Query& first, const Query& second){
            const int first_block = first.left / block_size;
            const int second_block = second.left / block_size;
            if(first_block != second_block){
                return first_block < second_block;
            }
            if(first_block & 1){
                return first.right > second.right;
            }
            return first.right < second.right;
        });

        std::vector<unsigned char> active(static_cast<std::size_t>(_n), 0);
        auto toggle = [&](int event_index){
            const int vertex = state->euler[static_cast<std::size_t>(event_index)];
            unsigned char& present = active[static_cast<std::size_t>(vertex)];
            if(present){
                erase(vertex);
                present = 0;
            }else{
                add(vertex);
                present = 1;
            }
        };

        int left = 0;
        int right = 0;
        for(const Query& query: order){
            while(query.left < left){
                toggle(--left);
            }
            while(right < query.right){
                toggle(right++);
            }
            while(left < query.left){
                toggle(left++);
            }
            while(query.right < right){
                toggle(--right);
            }
            if(query.extra != -1){
                add(query.extra);
            }
            answer(query.index);
            if(query.extra != -1){
                erase(query.extra);
            }
        }
        for(int vertex = 0; vertex < _n; ++vertex){
            if(active[static_cast<std::size_t>(vertex)]){
                erase(vertex);
            }
        }
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_TREE_MO_HPP_INCLUDED
