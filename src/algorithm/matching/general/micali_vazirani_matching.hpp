#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_MICALI_VAZIRANI_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_MICALI_VAZIRANI_MATCHING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct MicaliVaziraniMatchingResult{
    int cardinality;
    std::vector<int> mate;
};

namespace micali_vazirani_matching_detail{

class IncrementalTreeSetUnion{
    struct NodeInfo{
        int micro = -1;
        int number = 0;
        int tree_parent = -1;
        int depth = 0;
        bool active = false;
        bool linked = false;
    };

    struct Microset{
        int root = -1;
        std::vector<int> nodes;
        std::uint64_t parent_code = 0;
        std::uint64_t mark_code = 0;
    };

    struct NewMicroset{
        int root;
        std::vector<int> nodes;
    };

    int capacity_;
    int block_size_;
    int nodes_per_block_;
    int parent_field_bits_;
    std::uint64_t parent_field_mask_;
    std::vector<unsigned char> answer_;
    std::vector<NodeInfo> node_;
    std::vector<Microset> microsets_;
    std::vector<int> local_number_;

    std::vector<int> macro_parent_;
    std::vector<int> macro_size_;
    std::vector<int> macro_top_;
    std::vector<char> macro_active_;

    static int ceil_log2(int value){
        int result = 0;
        int power = 1;
        while(power < value){
            power <<= 1;
            result++;
        }
        return result;
    }

    static bool table_cell_count(int block_size, std::size_t& result){
        const int count = block_size - 1;
        const int field_bits = ceil_log2(block_size);
        const int total_bits = count * (field_bits + 1);
        if(total_bits < 0 || 62 < total_bits) return false;
        const std::uint64_t states = std::uint64_t{1} << total_bits;
        if(states > std::numeric_limits<std::size_t>::max()
                / static_cast<std::size_t>(count)){
            return false;
        }
        result = static_cast<std::size_t>(states)
            * static_cast<std::size_t>(count);
        return true;
    }

    void choose_block_size(){
        block_size_ = 4;
        const std::size_t budget = std::max<std::size_t>(
            1536, static_cast<std::size_t>(capacity_)
        );
        for(int candidate = 5; candidate <= 63; candidate++){
            std::size_t cells = 0;
            if(!table_cell_count(candidate, cells) || budget < cells) break;
            block_size_ = candidate;
        }
        nodes_per_block_ = block_size_ - 1;
        parent_field_bits_ = ceil_log2(block_size_);
        parent_field_mask_ =
            (std::uint64_t{1} << parent_field_bits_) - 1;
    }

    void build_answer_table(){
        std::size_t cell_count = 0;
        if(!table_cell_count(block_size_, cell_count)){
            throw std::length_error(
                "MicaliVaziraniMatching: tree-set-union table is too large"
            );
        }
        answer_.assign(cell_count, 0);

        const int count = nodes_per_block_;
        const int parent_bits = count * parent_field_bits_;
        const std::uint64_t parent_table_count =
            std::uint64_t{1} << parent_bits;
        const std::uint64_t mark_table_count =
            std::uint64_t{1} << count;
        std::vector<int> parent(static_cast<std::size_t>(count + 1), 0);
        std::vector<int> memo(static_cast<std::size_t>(count + 1), -1);
        std::vector<int> path;
        path.reserve(static_cast<std::size_t>(count));

        for(std::uint64_t parent_code = 0;
                parent_code < parent_table_count; parent_code++){
            bool valid = true;
            for(int number = 1; number <= count; number++){
                const int value = static_cast<int>(
                    (parent_code
                        >> ((number - 1) * parent_field_bits_))
                    & parent_field_mask_
                );
                parent[static_cast<std::size_t>(number)] = value;
                if(count < value) valid = false;
            }
            if(valid){
                for(int start = 1; start <= count && valid; start++){
                    int current = start;
                    for(int steps = 0; current != 0 && steps <= count;
                            steps++){
                        current = parent[static_cast<std::size_t>(current)];
                        if(steps == count) valid = false;
                    }
                }
            }
            if(!valid) continue;

            for(std::uint64_t marks = 0; marks < mark_table_count; marks++){
                std::fill(memo.begin(), memo.end(), -1);
                memo[0] = 0;
                for(int start = 1; start <= count; start++){
                    if(memo[static_cast<std::size_t>(start)] != -1) continue;
                    path.clear();
                    int current = start;
                    while(current != 0
                            && memo[static_cast<std::size_t>(current)] == -1){
                        path.push_back(current);
                        memo[static_cast<std::size_t>(current)] = -2;
                        current = parent[static_cast<std::size_t>(current)];
                    }
                    int nearest = memo[static_cast<std::size_t>(current)];
                    for(auto iterator = path.rbegin(); iterator != path.rend();
                            ++iterator){
                        const int vertex = *iterator;
                        if(((marks >> (vertex - 1)) & 1U) == 0) nearest = vertex;
                        memo[static_cast<std::size_t>(vertex)] = nearest;
                    }
                }
                const std::uint64_t state =
                    (parent_code << count) | marks;
                const std::size_t offset = static_cast<std::size_t>(state)
                    * static_cast<std::size_t>(count);
                for(int number = 1; number <= count; number++){
                    answer_[offset + static_cast<std::size_t>(number - 1)] =
                        static_cast<unsigned char>(
                            memo[static_cast<std::size_t>(number)]
                        );
                }
            }
        }
    }

    void macro_make(int vertex){
        if(macro_active_[static_cast<std::size_t>(vertex)]) return;
        macro_active_[static_cast<std::size_t>(vertex)] = 1;
        macro_parent_[static_cast<std::size_t>(vertex)] = vertex;
        macro_size_[static_cast<std::size_t>(vertex)] = 1;
        macro_top_[static_cast<std::size_t>(vertex)] = vertex;
    }

    int macro_representative(int vertex){
        int root = vertex;
        while(macro_parent_[static_cast<std::size_t>(root)] != root){
            root = macro_parent_[static_cast<std::size_t>(root)];
        }
        while(macro_parent_[static_cast<std::size_t>(vertex)] != root){
            const int next = macro_parent_[static_cast<std::size_t>(vertex)];
            macro_parent_[static_cast<std::size_t>(vertex)] = root;
            vertex = next;
        }
        return root;
    }

    int macro_name(int vertex){
        return macro_top_[static_cast<std::size_t>(macro_representative(vertex))];
    }

    void macro_unite(int first, int second){
        int left = macro_representative(first);
        int right = macro_representative(second);
        if(left == right) return;
        if(macro_size_[static_cast<std::size_t>(left)]
                < macro_size_[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        macro_parent_[static_cast<std::size_t>(right)] = left;
        macro_size_[static_cast<std::size_t>(left)] +=
            macro_size_[static_cast<std::size_t>(right)];
        const int left_top = macro_top_[static_cast<std::size_t>(left)];
        const int right_top = macro_top_[static_cast<std::size_t>(right)];
        if(node_[static_cast<std::size_t>(right_top)].depth
                < node_[static_cast<std::size_t>(left_top)].depth){
            macro_top_[static_cast<std::size_t>(left)] = right_top;
        }
    }

    int micro_find(int vertex) const{
        const NodeInfo& info = node_[static_cast<std::size_t>(vertex)];
        const Microset& micro =
            microsets_[static_cast<std::size_t>(info.micro)];
        const std::uint64_t state =
            (micro.parent_code << nodes_per_block_) | micro.mark_code;
        const std::size_t offset = static_cast<std::size_t>(state)
            * static_cast<std::size_t>(nodes_per_block_)
            + static_cast<std::size_t>(info.number - 1);
        const int answer = answer_[offset];
        if(answer == 0) return micro.root;
        return micro.nodes[static_cast<std::size_t>(answer - 1)];
    }

    bool same_micro(int left, int right) const{
        return 0 <= left && 0 <= right
            && node_[static_cast<std::size_t>(left)].micro
                == node_[static_cast<std::size_t>(right)].micro;
    }

    void install_microset(int identifier, NewMicroset group){
        Microset micro;
        micro.root = group.root;
        micro.nodes = std::move(group.nodes);
        for(std::size_t index = 0; index < micro.nodes.size(); index++){
            local_number_[static_cast<std::size_t>(micro.nodes[index])] =
                static_cast<int>(index) + 1;
        }
        for(std::size_t index = 0; index < micro.nodes.size(); index++){
            const int vertex = micro.nodes[index];
            NodeInfo& info = node_[static_cast<std::size_t>(vertex)];
            info.micro = identifier;
            info.number = static_cast<int>(index) + 1;
            const int parent_vertex = info.tree_parent;
            const int parent_number = parent_vertex < 0
                ? 0
                : local_number_[static_cast<std::size_t>(parent_vertex)];
            micro.parent_code |=
                static_cast<std::uint64_t>(parent_number)
                << (index * static_cast<std::size_t>(parent_field_bits_));
            if(info.linked){
                micro.mark_code |= std::uint64_t{1} << index;
            }
        }
        for(int vertex: micro.nodes){
            local_number_[static_cast<std::size_t>(vertex)] = 0;
        }
        if(identifier == static_cast<int>(microsets_.size())){
            microsets_.push_back(std::move(micro));
        }else{
            microsets_[static_cast<std::size_t>(identifier)] = std::move(micro);
        }
        if(0 <= group.root) macro_make(group.root);
    }

    void split_microset(int identifier){
        const int old_root =
            microsets_[static_cast<std::size_t>(identifier)].root;
        const std::vector<int> old_nodes =
            microsets_[static_cast<std::size_t>(identifier)].nodes;
        const int count = static_cast<int>(old_nodes.size());

        for(int index = 0; index < count; index++){
            local_number_[static_cast<std::size_t>(old_nodes[index])] = index + 1;
        }
        std::vector<std::vector<int>> children(static_cast<std::size_t>(count));
        std::vector<int> roots;
        for(int index = 0; index < count; index++){
            const int parent_vertex =
                node_[static_cast<std::size_t>(old_nodes[index])].tree_parent;
            const int parent_number = parent_vertex < 0
                ? 0
                : local_number_[static_cast<std::size_t>(parent_vertex)];
            if(parent_number == 0){
                roots.push_back(index);
            }else{
                children[static_cast<std::size_t>(parent_number - 1)]
                    .push_back(index);
            }
        }
        for(int vertex: old_nodes){
            local_number_[static_cast<std::size_t>(vertex)] = 0;
        }

        std::vector<int> order;
        order.reserve(static_cast<std::size_t>(count));
        std::vector<int> stack = roots;
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            order.push_back(vertex);
            for(int child: children[static_cast<std::size_t>(vertex)]){
                stack.push_back(child);
            }
        }
        std::reverse(order.begin(), order.end());

        const int threshold = std::max(2, (block_size_ + 2) / 4);
        std::vector<int> next(static_cast<std::size_t>(count), -1);
        std::vector<int> head(static_cast<std::size_t>(count), -1);
        std::vector<int> tail(static_cast<std::size_t>(count), -1);
        std::vector<int> residual_size(static_cast<std::size_t>(count), 0);
        std::vector<NewMicroset> groups;

        auto materialize = [&](int root, int first){
            NewMicroset group{root, {}};
            for(int current = first; current != -1;
                    current = next[static_cast<std::size_t>(current)]){
                group.nodes.push_back(old_nodes[static_cast<std::size_t>(current)]);
            }
            groups.push_back(std::move(group));
        };

        for(int vertex: order){
            head[static_cast<std::size_t>(vertex)] = vertex;
            tail[static_cast<std::size_t>(vertex)] = vertex;
            residual_size[static_cast<std::size_t>(vertex)] = 1;
            for(int child: children[static_cast<std::size_t>(vertex)]){
                next[static_cast<std::size_t>(
                    tail[static_cast<std::size_t>(vertex)]
                )] = head[static_cast<std::size_t>(child)];
                tail[static_cast<std::size_t>(vertex)] =
                    tail[static_cast<std::size_t>(child)];
                residual_size[static_cast<std::size_t>(vertex)] +=
                    residual_size[static_cast<std::size_t>(child)];
                if(threshold
                        <= residual_size[static_cast<std::size_t>(vertex)]){
                    const int first = next[static_cast<std::size_t>(vertex)];
                    next[static_cast<std::size_t>(vertex)] = -1;
                    materialize(old_nodes[static_cast<std::size_t>(vertex)], first);
                    tail[static_cast<std::size_t>(vertex)] = vertex;
                    residual_size[static_cast<std::size_t>(vertex)] = 1;
                }
            }
        }

        int external_head = -1;
        int external_tail = -1;
        int external_size = 1;
        for(int root_index: roots){
            if(external_head == -1){
                external_head = head[static_cast<std::size_t>(root_index)];
            }else{
                next[static_cast<std::size_t>(external_tail)] =
                    head[static_cast<std::size_t>(root_index)];
            }
            external_tail = tail[static_cast<std::size_t>(root_index)];
            external_size += residual_size[static_cast<std::size_t>(root_index)];
            if(threshold <= external_size){
                materialize(old_root, external_head);
                external_head = -1;
                external_tail = -1;
                external_size = 1;
            }
        }
        if(external_head != -1) materialize(old_root, external_head);

        int reuse = -1;
        for(int index = static_cast<int>(groups.size()) - 1; 0 <= index; index--){
            if(groups[static_cast<std::size_t>(index)].root == old_root){
                reuse = index;
                break;
            }
        }
        if(reuse == -1){
            throw std::logic_error(
                "MicaliVaziraniMatching: invalid tree-set-union split"
            );
        }

        NewMicroset reused = std::move(groups[static_cast<std::size_t>(reuse)]);
        for(int index = 0; index < static_cast<int>(groups.size()); index++){
            if(index == reuse) continue;
            const int new_identifier = static_cast<int>(microsets_.size());
            install_microset(
                new_identifier, std::move(groups[static_cast<std::size_t>(index)])
            );
        }
        install_microset(identifier, std::move(reused));
    }

public:
    explicit IncrementalTreeSetUnion(int capacity)
        : capacity_(capacity), block_size_(0), nodes_per_block_(0),
          parent_field_bits_(0), parent_field_mask_(0),
          node_(static_cast<std::size_t>(capacity)),
          local_number_(static_cast<std::size_t>(capacity), 0),
          macro_parent_(static_cast<std::size_t>(capacity), -1),
          macro_size_(static_cast<std::size_t>(capacity), 0),
          macro_top_(static_cast<std::size_t>(capacity), -1),
          macro_active_(static_cast<std::size_t>(capacity), 0){
        choose_block_size();
        build_answer_table();
    }

    void reset(int root){
        std::fill(node_.begin(), node_.end(), NodeInfo{});
        std::fill(macro_parent_.begin(), macro_parent_.end(), -1);
        std::fill(macro_size_.begin(), macro_size_.end(), 0);
        std::fill(macro_top_.begin(), macro_top_.end(), -1);
        std::fill(macro_active_.begin(), macro_active_.end(), 0);
        microsets_.clear();
        Microset initial;
        initial.nodes.push_back(root);
        microsets_.push_back(std::move(initial));
        NodeInfo& info = node_[static_cast<std::size_t>(root)];
        info.micro = 0;
        info.number = 1;
        info.depth = 0;
        info.active = true;
    }

    void grow(int parent, int child){
        NodeInfo& parent_info = node_[static_cast<std::size_t>(parent)];
        NodeInfo& child_info = node_[static_cast<std::size_t>(child)];
        if(!parent_info.active || child_info.active){
            throw std::logic_error(
                "MicaliVaziraniMatching: invalid tree-set-union grow"
            );
        }
        const int identifier = parent_info.micro;
        Microset& micro = microsets_[static_cast<std::size_t>(identifier)];
        const int number = static_cast<int>(micro.nodes.size()) + 1;
        child_info.micro = identifier;
        child_info.number = number;
        child_info.tree_parent = parent;
        child_info.depth = parent_info.depth + 1;
        child_info.active = true;
        micro.nodes.push_back(child);
        if(number <= nodes_per_block_){
            micro.parent_code |=
                static_cast<std::uint64_t>(parent_info.number)
                << ((number - 1) * parent_field_bits_);
        }
        if(static_cast<int>(micro.nodes.size()) == block_size_){
            split_microset(identifier);
        }
    }

    void link(int vertex){
        NodeInfo& info = node_[static_cast<std::size_t>(vertex)];
        if(!info.active || info.linked || info.tree_parent == -1){
            throw std::logic_error(
                "MicaliVaziraniMatching: invalid tree-set-union link"
            );
        }
        info.linked = true;
        Microset& micro = microsets_[static_cast<std::size_t>(info.micro)];
        micro.mark_code |= std::uint64_t{1} << (info.number - 1);
    }

    int find(int vertex){
        int answer = micro_find(vertex);
        if(!same_micro(vertex, answer)){
            if(answer < 0 || !macro_active_[static_cast<std::size_t>(answer)]){
                throw std::logic_error(
                    "MicaliVaziraniMatching: invalid tree-set-union root"
                );
            }
            int current = macro_name(answer);
            while(true){
                answer = micro_find(current);
                if(same_micro(current, answer)) break;
                if(answer < 0
                        || !macro_active_[static_cast<std::size_t>(answer)]){
                    throw std::logic_error(
                        "MicaliVaziraniMatching: invalid tree-set-union ascent"
                    );
                }
                macro_unite(answer, current);
                current = macro_name(answer);
            }
        }
        return answer;
    }
};

enum class EdgeKind: unsigned char{
    not_scanned,
    prop,
    bridge
};

struct Edge{
    int to;
    int reverse_index;
    EdgeKind kind;
};

struct BridgeRecord{
    int original_left = -1;
    int original_right = -1;
    int bud_left = -1;
    int bud_right = -1;
};

class Solver{
    static constexpr std::size_t infinity =
        std::numeric_limits<std::size_t>::max() / 4;

    enum class FrameKind: unsigned char{
        augment,
        opening,
        handle_child,
        restore_result
    };

    struct Frame{
        FrameKind kind;
        int first = -1;
        int second = -1;
        int third = -1;
        std::size_t index = 0;
        std::size_t next_index = 0;
        bool initial = false;
    };

    int n_;
    int artificial_root_;
    std::vector<std::vector<Edge>> adjacency_;
    std::vector<int> mate_;

    std::vector<std::vector<int>> predecessors_;
    std::vector<std::size_t> predecessor_cursor_;
    std::vector<char> removed_;
    std::vector<std::size_t> even_level_;
    std::vector<std::size_t> odd_level_;
    IncrementalTreeSetUnion buds_;
    std::vector<int> petal_parent_;
    std::vector<int> temporary_bud_;
    std::uint64_t color_counter_ = 0;
    std::vector<std::uint64_t> color_;
    std::vector<std::vector<std::pair<int, int>>> ddfs_children_;
    std::vector<BridgeRecord> creating_bridge_;
    std::vector<int> removal_queue_;
    std::size_t removal_head_ = 0;

    std::size_t minimum_level(int vertex) const{
        return std::min(
            even_level_[static_cast<std::size_t>(vertex)],
            odd_level_[static_cast<std::size_t>(vertex)]
        );
    }

    std::size_t safe_tenacity_sum(
        std::size_t left, std::size_t right
    ) const{
        if(infinity <= left || infinity <= right
                || infinity - 1 <= left
                || infinity - left - 1 <= right){
            return infinity;
        }
        return left + right + 1;
    }

    std::size_t edge_tenacity(int left, int right) const{
        if(mate_[static_cast<std::size_t>(left)] == right){
            return safe_tenacity_sum(
                odd_level_[static_cast<std::size_t>(left)],
                odd_level_[static_cast<std::size_t>(right)]
            );
        }
        return safe_tenacity_sum(
            even_level_[static_cast<std::size_t>(left)],
            even_level_[static_cast<std::size_t>(right)]
        );
    }

    int bud(int vertex){
        const int current = buds_.find(vertex);
        const int temporary = temporary_bud_[static_cast<std::size_t>(current)];
        return temporary == -1 ? current : temporary;
    }

    void set_level(
        int vertex,
        std::size_t level,
        std::vector<std::vector<int>>& vertices_at_level
    ){
        if((level & 1U) == 0){
            even_level_[static_cast<std::size_t>(vertex)] = level;
        }else{
            odd_level_[static_cast<std::size_t>(vertex)] = level;
        }
        if(level < vertices_at_level.size()){
            vertices_at_level[level].push_back(vertex);
        }
    }

    void remove_vertex(int vertex){
        if(removed_[static_cast<std::size_t>(vertex)]) return;
        removed_[static_cast<std::size_t>(vertex)] = 1;
        removal_queue_.push_back(vertex);
    }

    void flip(int left, int right){
        if(removed_[static_cast<std::size_t>(left)]
                || removed_[static_cast<std::size_t>(right)]
                || mate_[static_cast<std::size_t>(left)] == right){
            return;
        }
        remove_vertex(left);
        remove_vertex(right);
        mate_[static_cast<std::size_t>(left)] = right;
        mate_[static_cast<std::size_t>(right)] = left;
    }

    void reset_phase(){
        for(auto& edges: adjacency_){
            for(Edge& edge: edges) edge.kind = EdgeKind::not_scanned;
        }
        for(auto& list: predecessors_) list.clear();
        std::fill(predecessor_cursor_.begin(), predecessor_cursor_.end(), 0);
        std::fill(removed_.begin(), removed_.end(), 0);
        std::fill(even_level_.begin(), even_level_.end(), infinity);
        std::fill(odd_level_.begin(), odd_level_.end(), infinity);
        std::fill(petal_parent_.begin(), petal_parent_.end(), -1);
        std::fill(temporary_bud_.begin(), temporary_bud_.end(), -1);
        std::fill(color_.begin(), color_.end(), 0);
        for(auto& list: ddfs_children_) list.clear();
        std::fill(
            creating_bridge_.begin(), creating_bridge_.end(), BridgeRecord{}
        );
        removal_queue_.clear();
        removal_head_ = 0;
        color_counter_ = 0;
        buds_.reset(artificial_root_);
    }

    void scan_level(
        std::size_t level,
        std::vector<std::vector<int>>& vertices_at_level,
        std::vector<std::vector<std::pair<int, int>>>& bridges
    ){
        for(int vertex: vertices_at_level[level]){
            const std::size_t degree =
                adjacency_[static_cast<std::size_t>(vertex)].size();
            for(std::size_t index = 0; index < degree; index++){
                Edge& edge = adjacency_[static_cast<std::size_t>(vertex)][index];
                if(edge.kind != EdgeKind::not_scanned) continue;
                const int to = edge.to;
                const bool odd_side =
                    odd_level_[static_cast<std::size_t>(vertex)] == level;
                const bool matched = mate_[static_cast<std::size_t>(vertex)] == to;
                if(odd_side != matched) continue;

                const int reverse = edge.reverse_index;
                Edge& reverse_edge =
                    adjacency_[static_cast<std::size_t>(to)]
                              [static_cast<std::size_t>(reverse)];
                if(level < minimum_level(to)){
                    edge.kind = EdgeKind::prop;
                    reverse_edge.kind = EdgeKind::prop;
                    if(level + 1 < minimum_level(to)){
                        buds_.grow(bud(vertex), to);
                        set_level(to, level + 1, vertices_at_level);
                    }
                    predecessors_[static_cast<std::size_t>(to)].push_back(vertex);
                }else{
                    edge.kind = EdgeKind::bridge;
                    reverse_edge.kind = EdgeKind::bridge;
                    const std::size_t tenacity = edge_tenacity(vertex, to);
                    if(tenacity < bridges.size()){
                        bridges[tenacity].push_back({vertex, to});
                    }
                }
            }
        }
    }

    bool ddfs_move(
        std::vector<int>& first_stack,
        std::uint64_t first_color,
        std::vector<int>& second_stack,
        std::uint64_t second_color,
        std::vector<int>& support,
        int& bottleneck
    ){
        const int current = first_stack.back();
        std::size_t& cursor =
            predecessor_cursor_[static_cast<std::size_t>(current)];
        const std::vector<int>& predecessors =
            predecessors_[static_cast<std::size_t>(current)];
        while(cursor < predecessors.size()){
            const int predecessor = predecessors[cursor++];
            const int next = bud(predecessor);
            if(removed_[static_cast<std::size_t>(predecessor)]) continue;
            if(color_[static_cast<std::size_t>(next)] == 0){
                first_stack.push_back(next);
                support.push_back(next);
                ddfs_children_[static_cast<std::size_t>(current)]
                    .push_back({predecessor, next});
                color_[static_cast<std::size_t>(next)] = first_color;
                return false;
            }
            if(next == second_stack.back()){
                ddfs_children_[static_cast<std::size_t>(current)]
                    .push_back({predecessor, next});
            }
        }

        first_stack.pop_back();
        if(first_stack.empty()){
            if(second_stack.size() == 1){
                bottleneck = second_stack.front();
                color_[static_cast<std::size_t>(bottleneck)] = 0;
                return true;
            }
            const int transferred = second_stack.back();
            second_stack.pop_back();
            first_stack.push_back(transferred);
            color_[static_cast<std::size_t>(transferred)] = first_color;
            (void)second_color;
        }
        return false;
    }

    std::pair<int, int> ddfs(
        int edge_left, int edge_right, std::vector<int>& support
    ){
        const int red_root = bud(edge_left);
        const int green_root = bud(edge_right);
        if(red_root == green_root) return {red_root, green_root};

        std::vector<int> red_stack{red_root};
        std::vector<int> green_stack{green_root};
        support.push_back(red_root);
        support.push_back(green_root);
        const std::uint64_t red = ++color_counter_;
        const std::uint64_t green = ++color_counter_;
        color_[static_cast<std::size_t>(red_root)] = red;
        color_[static_cast<std::size_t>(green_root)] = green;

        while(true){
            if(minimum_level(red_stack.back()) == 0
                    && minimum_level(green_stack.back()) == 0){
                return {red_stack.back(), green_stack.back()};
            }
            int bottleneck = -1;
            bool found = false;
            if(minimum_level(green_stack.back())
                    <= minimum_level(red_stack.back())){
                found = ddfs_move(
                    red_stack, red, green_stack, green, support, bottleneck
                );
            }else{
                found = ddfs_move(
                    green_stack, green, red_stack, red, support, bottleneck
                );
            }
            if(found) return {bottleneck, bottleneck};
        }
    }

    bool handle_opening(
        int current,
        int current_bud,
        int target_bud,
        std::size_t child_index,
        std::vector<Frame>& stack
    ){
        if(current_bud == target_bud){
            stack.push_back({
                FrameKind::augment, current, current_bud, -1, 0, 0, false
            });
            return true;
        }
        const auto& children =
            ddfs_children_[static_cast<std::size_t>(current_bud)];
        for(std::size_t index = child_index; index < children.size(); index++){
            const auto [original, next_bud] = children[index];
            if(next_bud == target_bud
                    || color_[static_cast<std::size_t>(next_bud)]
                        == color_[static_cast<std::size_t>(current_bud)]){
                stack.push_back({
                    FrameKind::handle_child,
                    current,
                    current_bud,
                    target_bud,
                    index + 1,
                    static_cast<std::size_t>(original),
                    false
                });
                stack.push_back({
                    FrameKind::opening,
                    original,
                    next_bud,
                    target_bud,
                    0,
                    0,
                    false
                });
                return false;
            }
        }
        return false;
    }

    void augment_even(int from, int target, std::vector<Frame>& stack){
        const std::vector<int>& from_predecessors =
            predecessors_[static_cast<std::size_t>(from)];
        if(from_predecessors.empty()){
            throw std::logic_error(
                "MicaliVaziraniMatching: invalid even-level predecessor"
            );
        }
        const int inner = from_predecessors.front();
        const std::vector<int>& inner_predecessors =
            predecessors_[static_cast<std::size_t>(inner)];
        std::size_t index = 0;
        while(index < inner_predecessors.size()
                && bud(inner_predecessors[index]) != bud(inner)){
            index++;
        }
        if(index == inner_predecessors.size()){
            throw std::logic_error(
                "MicaliVaziraniMatching: missing alternating predecessor"
            );
        }
        const int next = inner_predecessors[index];
        flip(inner, next);
        stack.push_back({
            FrameKind::augment, next, target, -1, 0, 0, false
        });
    }

    void augment_through_bridge(
        int from, int target, std::vector<Frame>& stack
    ){
        BridgeRecord bridge = creating_bridge_[static_cast<std::size_t>(from)];
        int original_left = bridge.original_left;
        int original_right = bridge.original_right;
        int left_bud = bridge.bud_left;
        int right_bud = bridge.bud_right;
        if((color_[static_cast<std::size_t>(left_bud)] ^ 1U)
                    == color_[static_cast<std::size_t>(from)]
                || color_[static_cast<std::size_t>(right_bud)]
                    == color_[static_cast<std::size_t>(from)]){
            std::swap(original_left, original_right);
            std::swap(left_bud, right_bud);
        }
        flip(original_left, original_right);
        const int parent = petal_parent_[static_cast<std::size_t>(from)];
        if(parent == -1){
            throw std::logic_error(
                "MicaliVaziraniMatching: missing petal parent"
            );
        }
        stack.push_back({
            FrameKind::augment, parent, target, -1, 0, 0, false
        });
        stack.push_back({
            FrameKind::opening,
            original_right,
            right_bud,
            parent,
            0,
            0,
            false
        });
        stack.push_back({
            FrameKind::opening,
            original_left,
            left_bud,
            from,
            0,
            0,
            false
        });
    }

    void augment_path(int left, int right){
        std::vector<Frame> stack;
        stack.push_back({
            FrameKind::augment, left, right, -1, 0, 0, true
        });
        bool opening_result = false;
        while(!stack.empty()){
            const Frame frame = stack.back();
            stack.pop_back();
            if(frame.kind == FrameKind::augment){
                if(frame.first == frame.second) continue;
                if(!frame.initial
                        && minimum_level(frame.first)
                            == even_level_[static_cast<std::size_t>(frame.first)]){
                    augment_even(frame.first, frame.second, stack);
                }else{
                    augment_through_bridge(frame.first, frame.second, stack);
                }
            }else if(frame.kind == FrameKind::opening){
                opening_result = handle_opening(
                    frame.first,
                    frame.second,
                    frame.third,
                    frame.index,
                    stack
                );
            }else if(frame.kind == FrameKind::handle_child){
                if(opening_result){
                    stack.push_back({FrameKind::restore_result});
                    stack.push_back({
                        FrameKind::augment,
                        frame.first,
                        frame.second,
                        -1,
                        0,
                        0,
                        false
                    });
                    flip(frame.second, static_cast<int>(frame.next_index));
                }else{
                    opening_result = handle_opening(
                        frame.first,
                        frame.second,
                        frame.third,
                        frame.index,
                        stack
                    );
                }
            }else{
                opening_result = true;
            }
        }
    }

    void propagate_removals(std::vector<std::size_t>& removed_predecessors){
        while(removal_head_ < removal_queue_.size()){
            const int vertex = removal_queue_[removal_head_++];
            for(const Edge& edge: adjacency_[static_cast<std::size_t>(vertex)]){
                if(edge.kind != EdgeKind::prop
                        || minimum_level(edge.to) <= minimum_level(vertex)
                        || removed_[static_cast<std::size_t>(edge.to)]){
                    continue;
                }
                std::size_t& count =
                    removed_predecessors[static_cast<std::size_t>(edge.to)];
                count++;
                if(count
                        == predecessors_[static_cast<std::size_t>(edge.to)].size()){
                    remove_vertex(edge.to);
                }
            }
        }
    }

    bool process_bridges(
        std::size_t level,
        std::vector<std::pair<int, int>> current_bridges,
        std::vector<std::vector<std::pair<int, int>>>& bridges,
        std::vector<std::vector<int>>& vertices_at_level,
        std::vector<std::size_t>& removed_predecessors
    ){
        bool augmented = false;
        for(const auto [edge_left, edge_right]: current_bridges){
            const int start_left = bud(edge_left);
            const int start_right = bud(edge_right);
            if(removed_[static_cast<std::size_t>(start_left)]
                    || removed_[static_cast<std::size_t>(start_right)]){
                continue;
            }
            std::vector<int> support;
            const auto result = ddfs(edge_left, edge_right, support);
            const BridgeRecord record{
                edge_left, edge_right, start_left, start_right
            };

            if(result.first == result.second){
                const int bottleneck = result.first;
                for(int vertex: support){
                    if(vertex == bottleneck) continue;
                    creating_bridge_[static_cast<std::size_t>(vertex)] = record;
                    petal_parent_[static_cast<std::size_t>(vertex)] = bottleneck;
                    buds_.link(vertex);
                }
                for(int vertex: support){
                    if(vertex == bottleneck) continue;
                    const std::size_t new_level =
                        2 * level + 1 - minimum_level(vertex);
                    set_level(vertex, new_level, vertices_at_level);
                    if(odd_level_[static_cast<std::size_t>(vertex)]
                            < even_level_[static_cast<std::size_t>(vertex)]){
                        for(const Edge& edge:
                                adjacency_[static_cast<std::size_t>(vertex)]){
                            if(edge.kind != EdgeKind::bridge
                                    || mate_[static_cast<std::size_t>(vertex)]
                                        == edge.to){
                                continue;
                            }
                            const std::size_t tenacity =
                                edge_tenacity(vertex, edge.to);
                            if(tenacity < bridges.size()){
                                bridges[tenacity].push_back({vertex, edge.to});
                            }
                        }
                    }
                }
                continue;
            }

            for(int vertex: support){
                if(vertex == result.second) continue;
                creating_bridge_[static_cast<std::size_t>(vertex)] = record;
                petal_parent_[static_cast<std::size_t>(vertex)] = result.second;
                temporary_bud_[static_cast<std::size_t>(vertex)] = result.second;
            }
            augment_path(result.first, result.second);
            augmented = true;
            propagate_removals(removed_predecessors);
            for(int vertex: support){
                if(!removed_[static_cast<std::size_t>(vertex)]){
                    temporary_bud_[static_cast<std::size_t>(vertex)] = -1;
                    petal_parent_[static_cast<std::size_t>(vertex)] = -1;
                    creating_bridge_[static_cast<std::size_t>(vertex)] =
                        BridgeRecord{};
                }
            }
        }
        return augmented;
    }

    bool run_phase(){
        reset_phase();
        std::vector<std::vector<int>> vertices_at_level(
            static_cast<std::size_t>(n_)
        );
        std::vector<std::vector<std::pair<int, int>>> bridges(
            static_cast<std::size_t>(2) * static_cast<std::size_t>(n_) + 2
        );
        std::vector<std::size_t> removed_predecessors(
            static_cast<std::size_t>(n_), 0
        );
        for(int vertex = 0; vertex < n_; vertex++){
            if(mate_[static_cast<std::size_t>(vertex)] == -1){
                buds_.grow(artificial_root_, vertex);
                set_level(vertex, 0, vertices_at_level);
            }
        }

        for(std::size_t level = 0; level < static_cast<std::size_t>(n_);
                level++){
            scan_level(level, vertices_at_level, bridges);
            const std::size_t tenacity = 2 * level + 1;
            std::vector<std::pair<int, int>> current =
                std::move(bridges[tenacity]);
            if(process_bridges(
                    level,
                    std::move(current),
                    bridges,
                    vertices_at_level,
                    removed_predecessors
                )){
                return true;
            }
        }
        return false;
    }

public:
    explicit Solver(int vertex_count)
        : n_(vertex_count), artificial_root_(vertex_count),
          adjacency_(static_cast<std::size_t>(vertex_count)),
          mate_(static_cast<std::size_t>(vertex_count), -1),
          predecessors_(static_cast<std::size_t>(vertex_count)),
          predecessor_cursor_(static_cast<std::size_t>(vertex_count), 0),
          removed_(static_cast<std::size_t>(vertex_count), 0),
          even_level_(static_cast<std::size_t>(vertex_count), infinity),
          odd_level_(static_cast<std::size_t>(vertex_count), infinity),
          buds_(vertex_count + 1),
          petal_parent_(static_cast<std::size_t>(vertex_count), -1),
          temporary_bud_(static_cast<std::size_t>(vertex_count), -1),
          color_(static_cast<std::size_t>(vertex_count), 0),
          ddfs_children_(static_cast<std::size_t>(vertex_count)),
          creating_bridge_(static_cast<std::size_t>(vertex_count)){}

    void add_edge(int left, int right){
        const int left_index =
            static_cast<int>(adjacency_[static_cast<std::size_t>(left)].size());
        const int right_index =
            static_cast<int>(adjacency_[static_cast<std::size_t>(right)].size());
        adjacency_[static_cast<std::size_t>(left)].push_back({
            right, right_index, EdgeKind::not_scanned
        });
        adjacency_[static_cast<std::size_t>(right)].push_back({
            left, left_index, EdgeKind::not_scanned
        });
    }

    std::vector<int> solve(){
        while(run_phase()){}
        return mate_;
    }
};

}  // namespace micali_vazirani_matching_detail

class MicaliVaziraniMatching{
    int vertex_count_;
    std::vector<std::pair<int, int>> edges_;

public:
    explicit MicaliVaziraniMatching(int vertex_count)
        : vertex_count_(vertex_count < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation "
                "(MicaliVaziraniMatching)."
            )
            : vertex_count){}

    int vertex_count() const noexcept{
        return vertex_count_;
    }

    std::size_t edge_count() const noexcept{
        return edges_.size();
    }

    void add_edge(int left, int right){
        if(left < 0 || vertex_count_ <= left
                || right < 0 || vertex_count_ <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(MicaliVaziraniMatching::add_edge)."
            );
        }
        if(left == right) return;
        edges_.push_back({left, right});
    }

    MicaliVaziraniMatchingResult solve() const{
        std::vector<int> result(static_cast<std::size_t>(vertex_count_), -1);
        if(edges_.empty()) return {0, std::move(result)};

        std::vector<char> active(static_cast<std::size_t>(vertex_count_), 0);
        for(const auto [left, right]: edges_){
            active[static_cast<std::size_t>(left)] = 1;
            active[static_cast<std::size_t>(right)] = 1;
        }
        std::vector<int> index(static_cast<std::size_t>(vertex_count_), -1);
        std::vector<int> original;
        original.reserve(static_cast<std::size_t>(vertex_count_));
        for(int vertex = 0; vertex < vertex_count_; vertex++){
            if(!active[static_cast<std::size_t>(vertex)]) continue;
            index[static_cast<std::size_t>(vertex)] =
                static_cast<int>(original.size());
            original.push_back(vertex);
        }

        micali_vazirani_matching_detail::Solver solver(
            static_cast<int>(original.size())
        );
        for(const auto [left, right]: edges_){
            solver.add_edge(
                index[static_cast<std::size_t>(left)],
                index[static_cast<std::size_t>(right)]
            );
        }
        const std::vector<int> compact_mate = solver.solve();
        int cardinality = 0;
        for(std::size_t vertex = 0; vertex < compact_mate.size(); vertex++){
            const int mate = compact_mate[vertex];
            if(mate == -1) continue;
            result[static_cast<std::size_t>(original[vertex])] =
                original[static_cast<std::size_t>(mate)];
            if(static_cast<int>(vertex) < mate) cardinality++;
        }
        return {cardinality, std::move(result)};
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_MICALI_VAZIRANI_MATCHING_HPP_INCLUDED
