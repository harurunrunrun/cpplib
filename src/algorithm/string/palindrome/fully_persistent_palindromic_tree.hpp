#ifndef CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_FULLY_PERSISTENT_PALINDROMIC_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_FULLY_PERSISTENT_PALINDROMIC_TREE_HPP_INCLUDED

#include "../../../structure/array/static_or_vector.hpp"

#include <array>
#include <bit>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<int ALPHABET, int MAX_NODES = -1, int MAX_VERSIONS = -1, char OFFSET = 'a'>
struct FullyPersistentPalindromicTree{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES == -1 || MAX_NODES >= 2);
    static_assert(MAX_VERSIONS >= -1);

    struct Node{
        std::array<int, ALPHABET> next{};
        int link = 0;
        int length = 0;
        int diff = 0;
        int series_link = 0;
        int suffix_count = 0;
        int first_version = 0;

        Node(){ next.fill(-1); }
    };

private:
    static constexpr bool dynamic_storage = MAX_NODES < 0 || MAX_VERSIONS < 0;
    std::vector<int> present_limit_;

    int presence_limit(int version) const{
        if constexpr(dynamic_storage) return present_limit_[static_cast<std::size_t>(version)];
        else return MAX_NODES;
    }
    int ancestor_jump(int version, int bit) const{
        const auto& row = ancestor[static_cast<std::size_t>(version)];
        return static_cast<std::size_t>(bit) < row.size() ? row[static_cast<std::size_t>(bit)] : 0;
    }

    static constexpr int VERSION_LOG = []{
        int result = 1;
        unsigned long long size = static_cast<unsigned long long>(MAX_VERSIONS) + 1;
        while((1ULL << result) < size && result < 63) ++result;
        return result;
    }();
    static constexpr int SET_HEIGHT = []{
        int result = 0;
        int size = 1;
        while(size < MAX_NODES){
            size <<= 1;
            ++result;
        }
        return result;
    }();
    static constexpr std::size_t SET_CAPACITY =
        static_cast<std::size_t>(MAX_VERSIONS) * (SET_HEIGHT + 1);

    struct SetNode{
        int left = -1;
        int right = -1;
        bool value = false;
    };

    int used = 2;
    int version_count = 1;
    int set_used = 0;
    cpplib_detail::StaticOrVector<Node, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_NODES))> nodes{};
    cpplib_detail::StaticOrVector<SetNode, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(SET_CAPACITY))> set_nodes{};
    cpplib_detail::StaticOrVector<int, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> parent{};
    cpplib_detail::StaticOrVector<int, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> depth{};
    cpplib_detail::StaticOrVector<char, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> appended{};
    cpplib_detail::StaticOrVector<cpplib_detail::StaticOrVector<int, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(VERSION_LOG))>, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> ancestor{};
    cpplib_detail::StaticOrVector<int, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> last_at{};
    cpplib_detail::StaticOrVector<int, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> distinct_at{};
    cpplib_detail::StaticOrVector<int, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> longest_at{};
    cpplib_detail::StaticOrVector<long long, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> pal_substrings_at{};
    cpplib_detail::StaticOrVector<int, ((MAX_NODES < 0 || MAX_VERSIONS < 0) ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSIONS + 1))> present_root{};

    int char_id(char c) const{
        const int id = static_cast<int>(c - OFFSET);
        if(id < 0 || ALPHABET <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: character is out of alphabet.");
        }
        return id;
    }

    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (version).");
        }
    }

    const Node& checked_node(int node) const{
        if(node < 0 || used <= node)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (node).");
        }
        return nodes[static_cast<std::size_t>(node)];
    }

    int ancestor_at_depth(int version, int target_depth) const{
        int distance = depth[static_cast<std::size_t>(version)] - target_depth;
        for(int bit = 0; distance > 0; ++bit){
            if(distance & 1) version = ancestor_jump(version, bit);
            distance >>= 1;
        }
        return version;
    }

    char character_at(int version, int position) const{
        const int owner = ancestor_at_depth(version, position + 1);
        return appended[static_cast<std::size_t>(owner)];
    }

    int suffix_candidate(int node, int version, int new_depth, char c) const{
        while(true){
            const int length = nodes[static_cast<std::size_t>(node)].length;
            if(length == -1) return node;
            const int position = new_depth - 2 - length;
            if(position >= 0 && character_at(version, position) == c) return node;
            node = nodes[static_cast<std::size_t>(node)].link;
        }
    }

    bool set_contains(int root, int left, int right, int position) const{
        if(root == -1 || position < left || position >= right) return false;
        if(right - left == 1) return set_nodes[static_cast<std::size_t>(root)].value;
        const int middle = left + (right - left) / 2;
        if(position < middle){
            return set_contains(set_nodes[static_cast<std::size_t>(root)].left, left, middle, position);
        }
        return set_contains(set_nodes[static_cast<std::size_t>(root)].right, middle, right, position);
    }

    int clone_set_node(int source){
        if(set_used == std::numeric_limits<int>::max()) throw std::length_error("palindromic set index overflow");
        set_nodes.ensure(static_cast<std::size_t>(set_used) + 1);
        const int result = set_used++;
        set_nodes[static_cast<std::size_t>(result)] = source == -1 ? SetNode{} : set_nodes[static_cast<std::size_t>(source)];
        return result;
    }

    int set_insert(int source, int left, int right, int position){
        const int result = clone_set_node(source);
        if(right - left == 1){
            set_nodes[static_cast<std::size_t>(result)].value = true;
            return result;
        }
        const int middle = left + (right - left) / 2;
        if(position < middle){
            set_nodes[static_cast<std::size_t>(result)].left = set_insert(
                source == -1 ? -1 : set_nodes[static_cast<std::size_t>(source)].left,
                left,
                middle,
                position
            );
        }else{
            set_nodes[static_cast<std::size_t>(result)].right = set_insert(
                source == -1 ? -1 : set_nodes[static_cast<std::size_t>(source)].right,
                middle,
                right,
                position
            );
        }
        return result;
    }

    void collect_set_nodes(
        int root,
        int left,
        int right,
        std::vector<int>& result
    ) const{
        if(root == -1) return;
        if(right - left == 1){
            if(set_nodes[static_cast<std::size_t>(root)].value) result.push_back(left);
            return;
        }
        const int middle = left + (right - left) / 2;
        collect_set_nodes(set_nodes[static_cast<std::size_t>(root)].left, left, middle, result);
        collect_set_nodes(set_nodes[static_cast<std::size_t>(root)].right, middle, right, result);
    }

public:
    FullyPersistentPalindromicTree(){
        nodes.resize(2);
        parent.resize(1);
        depth.resize(1);
        appended.resize(1);
        ancestor.resize(1);
        last_at.resize(1);
        distinct_at.resize(1);
        longest_at.resize(1);
        pal_substrings_at.resize(1);
        present_root.resize(1);
        for(auto& row: ancestor) row.resize(1);
        if constexpr(dynamic_storage) present_limit_.push_back(2);
        nodes[0] = Node();
        nodes[1] = Node();
        nodes[0].length = -1;
        nodes[0].link = 0;
        nodes[1].length = 0;
        nodes[1].link = 0;
        parent.fill(-1);
        depth.fill(0);
        appended.fill(0);
        for(auto& row: ancestor) row.fill(0);
        last_at.fill(1);
        distinct_at.fill(0);
        longest_at.fill(1);
        pal_substrings_at.fill(0);
        present_root.fill(-1);
    }

    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int node_count() const{ return used; }
    int size(int version) const{
        check_version(version);
        return depth[static_cast<std::size_t>(version)];
    }
    int size() const{ return size(latest_version()); }

    const Node& operator[](int node) const{ return checked_node(node); }
    int length(int node) const{ return checked_node(node).length; }
    int link(int node) const{ return checked_node(node).link; }
    int diff(int node) const{ return checked_node(node).diff; }
    int series_link(int node) const{ return checked_node(node).series_link; }

    int append(int version, char c){
        check_version(version);
        if(MAX_VERSIONS >= 0 && version_count > MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (append).");
        }
        if(version_count == std::numeric_limits<int>::max()) throw std::length_error("palindromic version index overflow");
        const int id = char_id(c);
        const int new_depth = depth[static_cast<std::size_t>(version)] + 1;
        const int candidate = suffix_candidate(last_at[static_cast<std::size_t>(version)], version, new_depth, c);
        int next = nodes[static_cast<std::size_t>(candidate)].next[static_cast<std::size_t>(id)];
        const bool creates_node = next == -1;
        if(creates_node){
            if(used == MAX_NODES)[[unlikely]]{
                throw std::runtime_error("library assertion fault: node capacity exceeded (append).");
            }
            next = used;
        }
        const bool already_present = set_contains(
            present_root[static_cast<std::size_t>(version)],
            0,
            presence_limit(version),
            next
        );
        if(!dynamic_storage && !already_present && static_cast<std::size_t>(set_used) + SET_HEIGHT + 1 > SET_CAPACITY)[[unlikely]]{
            throw std::runtime_error("library assertion fault: set capacity exceeded (append).");
        }

        Node created;
        if(creates_node){
            created.length = nodes[static_cast<std::size_t>(candidate)].length + 2;
            created.first_version = version_count;
            if(created.length == 1){
                created.link = 1;
            }else{
                const int link_candidate = suffix_candidate(
                    nodes[static_cast<std::size_t>(candidate)].link,
                    version,
                    new_depth,
                    c
                );
                created.link = nodes[static_cast<std::size_t>(link_candidate)].next[static_cast<std::size_t>(id)];
                if(created.link == -1)[[unlikely]]{
                    throw std::logic_error("library assertion fault: missing suffix transition (append).");
                }
            }
            created.diff = created.length - nodes[static_cast<std::size_t>(created.link)].length;
            created.series_link = created.diff == nodes[static_cast<std::size_t>(created.link)].diff
                ? nodes[static_cast<std::size_t>(created.link)].series_link
                : created.link;
            created.suffix_count = nodes[static_cast<std::size_t>(created.link)].suffix_count + 1;
        }

        int new_root = present_root[static_cast<std::size_t>(version)];
        int limit = presence_limit(version);
        if constexpr(dynamic_storage){
            while(next >= limit){
                if(limit > std::numeric_limits<int>::max() / 2) throw std::length_error("palindromic node index overflow");
                const int expanded = clone_set_node(-1);
                set_nodes[static_cast<std::size_t>(expanded)].left = new_root;
                new_root = expanded;
                limit *= 2;
            }
        }
        if(!already_present) new_root = set_insert(new_root, 0, limit, next);
        if(creates_node){
            nodes.ensure(static_cast<std::size_t>(next) + 1);
            nodes[static_cast<std::size_t>(next)] = created;
            nodes[static_cast<std::size_t>(candidate)].next[static_cast<std::size_t>(id)] = next;
            ++used;
        }

        const int result = version_count++;
        parent.ensure(static_cast<std::size_t>(result) + 1);
        depth.ensure(static_cast<std::size_t>(result) + 1);
        appended.ensure(static_cast<std::size_t>(result) + 1);
        ancestor.ensure(static_cast<std::size_t>(result) + 1);
        last_at.ensure(static_cast<std::size_t>(result) + 1);
        distinct_at.ensure(static_cast<std::size_t>(result) + 1);
        longest_at.ensure(static_cast<std::size_t>(result) + 1);
        pal_substrings_at.ensure(static_cast<std::size_t>(result) + 1);
        present_root.ensure(static_cast<std::size_t>(result) + 1);
        if constexpr(dynamic_storage) present_limit_.push_back(limit);
        const int version_log = dynamic_storage ? static_cast<int>(std::bit_width(static_cast<unsigned>(new_depth))) : VERSION_LOG;
        ancestor[static_cast<std::size_t>(result)].resize(static_cast<std::size_t>(version_log));
        parent[static_cast<std::size_t>(result)] = version;
        depth[static_cast<std::size_t>(result)] = new_depth;
        appended[static_cast<std::size_t>(result)] = c;
        ancestor[static_cast<std::size_t>(result)][0] = version;
        for(int bit = 1; bit < version_log; ++bit){
            ancestor[static_cast<std::size_t>(result)][static_cast<std::size_t>(bit)] =
                ancestor_jump(ancestor_jump(result, bit - 1), bit - 1);
        }
        last_at[static_cast<std::size_t>(result)] = next;
        distinct_at[static_cast<std::size_t>(result)] =
            distinct_at[static_cast<std::size_t>(version)] + static_cast<int>(!already_present);
        const int previous_longest = longest_at[static_cast<std::size_t>(version)];
        longest_at[static_cast<std::size_t>(result)] =
            nodes[static_cast<std::size_t>(previous_longest)].length < nodes[static_cast<std::size_t>(next)].length
                ? next
                : previous_longest;
        pal_substrings_at[static_cast<std::size_t>(result)] =
            pal_substrings_at[static_cast<std::size_t>(version)] + nodes[static_cast<std::size_t>(next)].suffix_count;
        present_root[static_cast<std::size_t>(result)] = new_root;
        return result;
    }

    int append(char c){ return append(latest_version(), c); }
    int add(int version, char c){ return append(version, c); }
    int add(char c){ return append(c); }

    std::string str(int version) const{
        check_version(version);
        std::string result(static_cast<std::size_t>(depth[static_cast<std::size_t>(version)]), '\0');
        for(int position = static_cast<int>(result.size()) - 1; position >= 0; --position){
            result[static_cast<std::size_t>(position)] = appended[static_cast<std::size_t>(version)];
            version = parent[static_cast<std::size_t>(version)];
        }
        return result;
    }

    int last(int version) const{
        check_version(version);
        return last_at[static_cast<std::size_t>(version)];
    }
    int distinct_palindromes(int version) const{
        check_version(version);
        return distinct_at[static_cast<std::size_t>(version)];
    }
    long long count_palindromic_substrings(int version) const{
        check_version(version);
        return pal_substrings_at[static_cast<std::size_t>(version)];
    }
    int longest_suffix_palindrome_length(int version) const{
        return nodes[static_cast<std::size_t>(last(version))].length;
    }
    int longest_palindrome_node(int version) const{
        check_version(version);
        return longest_at[static_cast<std::size_t>(version)];
    }
    int longest_palindrome_length(int version) const{
        return nodes[static_cast<std::size_t>(longest_palindrome_node(version))].length;
    }

    bool node_present(int version, int node) const{
        check_version(version);
        checked_node(node);
        if(node < 2) return true;
        return set_contains(present_root[static_cast<std::size_t>(version)], 0, presence_limit(version), node);
    }

    int find_node(std::string_view value, int version) const{
        check_version(version);
        if(value.empty()) return 1;
        int node = value.size() % 2 == 0 ? 1 : 0;
        for(int left = (static_cast<int>(value.size()) - 1) / 2; left >= 0; --left){
            const int right = static_cast<int>(value.size()) - 1 - left;
            if(value[static_cast<std::size_t>(left)] != value[static_cast<std::size_t>(right)]) return -1;
            const int id = static_cast<int>(value[static_cast<std::size_t>(left)] - OFFSET);
            if(id < 0 || ALPHABET <= id) return -1;
            node = nodes[static_cast<std::size_t>(node)].next[static_cast<std::size_t>(id)];
            if(node == -1) return -1;
        }
        return node_present(version, node) ? node : -1;
    }
    bool contains(std::string_view value, int version) const{
        return find_node(value, version) != -1;
    }

    std::pair<int, int> first_occurrence_range(int node) const{
        const Node& current = checked_node(node);
        if(current.length < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: root does not represent a string.");
        }
        if(current.length == 0) return {0, 0};
        const int right = depth[static_cast<std::size_t>(current.first_version)];
        return {right - current.length, right};
    }
    std::string palindrome(int node) const{
        const Node& current = checked_node(node);
        if(current.length < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: root does not represent a string.");
        }
        if(current.length == 0) return {};
        const std::string text = str(current.first_version);
        return text.substr(text.size() - static_cast<std::size_t>(current.length));
    }

    std::vector<int> palindrome_nodes(int version) const{
        check_version(version);
        std::vector<int> result;
        result.reserve(static_cast<std::size_t>(distinct_at[static_cast<std::size_t>(version)]));
        collect_set_nodes(present_root[static_cast<std::size_t>(version)], 0, presence_limit(version), result);
        return result;
    }
    std::vector<std::string> palindromes(int version) const{
        std::vector<std::string> result;
        for(int node: palindrome_nodes(version)) result.push_back(palindrome(node));
        return result;
    }
    std::vector<int> suffix_palindrome_nodes(int version) const{
        check_version(version);
        std::vector<int> result;
        for(int node = last_at[static_cast<std::size_t>(version)]; node > 1; node = nodes[static_cast<std::size_t>(node)].link){
            result.push_back(node);
        }
        return result;
    }
    std::vector<int> suffix_palindrome_lengths(int version) const{
        std::vector<int> result;
        for(int node: suffix_palindrome_nodes(version)) result.push_back(nodes[static_cast<std::size_t>(node)].length);
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_FULLY_PERSISTENT_PALINDROMIC_TREE_HPP_INCLUDED
