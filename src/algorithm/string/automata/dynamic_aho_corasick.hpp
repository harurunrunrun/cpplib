#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_DYNAMIC_AHO_CORASICK_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_DYNAMIC_AHO_CORASICK_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

class DynamicAhoCorasick{
    struct StaticAutomaton{
        struct Node{
            std::map<unsigned char, int> next;
            int failure = 0;
            long long output = 0;
        };
        std::vector<Node> nodes;

        StaticAutomaton() = default;

        explicit StaticAutomaton(const std::vector<std::string>& patterns)
            : nodes(1){
            for(const std::string& pattern: patterns){
                int node = 0;
                for(unsigned char symbol: pattern){
                    auto& transition =
                        nodes[static_cast<std::size_t>(node)].next;
                    const auto iterator = transition.find(symbol);
                    if(iterator == transition.end()){
                        const int child = static_cast<int>(nodes.size());
                        transition.emplace(symbol, child);
                        nodes.emplace_back();
                        node = child;
                    }else{
                        node = iterator->second;
                    }
                }
                ++nodes[static_cast<std::size_t>(node)].output;
            }
            std::queue<int> queue;
            for(const auto [symbol, child]: nodes[0].next){
                static_cast<void>(symbol);
                queue.push(child);
            }
            while(!queue.empty()){
                const int node = queue.front();
                queue.pop();
                const int failure =
                    nodes[static_cast<std::size_t>(node)].failure;
                nodes[static_cast<std::size_t>(node)].output +=
                    nodes[static_cast<std::size_t>(failure)].output;
                for(const auto [symbol, child]:
                    nodes[static_cast<std::size_t>(node)].next){
                    int suffix = failure;
                    while(suffix != 0
                        && !nodes[static_cast<std::size_t>(suffix)]
                            .next.contains(symbol)){
                        suffix =
                            nodes[static_cast<std::size_t>(suffix)].failure;
                    }
                    const auto iterator =
                        nodes[static_cast<std::size_t>(suffix)]
                            .next.find(symbol);
                    if(iterator !=
                        nodes[static_cast<std::size_t>(suffix)].next.end()
                        && iterator->second != child){
                        nodes[static_cast<std::size_t>(child)].failure =
                            iterator->second;
                    }
                    queue.push(child);
                }
            }
        }

        long long count_matches(std::string_view text) const{
            if(nodes.empty()) return 0;
            long long result = nodes[0].output;
            int state = 0;
            for(unsigned char symbol: text){
                while(state != 0
                    && !nodes[static_cast<std::size_t>(state)]
                        .next.contains(symbol)){
                    state = nodes[static_cast<std::size_t>(state)].failure;
                }
                const auto iterator =
                    nodes[static_cast<std::size_t>(state)].next.find(symbol);
                if(iterator !=
                    nodes[static_cast<std::size_t>(state)].next.end()){
                    state = iterator->second;
                }
                result += nodes[static_cast<std::size_t>(state)].output;
            }
            return result;
        }
    };

    struct Bucket{
        std::vector<std::string> patterns;
        StaticAutomaton automaton;
    };

    std::vector<Bucket> positive_;
    std::vector<Bucket> negative_;
    std::unordered_map<std::string, int> multiplicity_;

    static void insert_bucket(
        std::vector<Bucket>& buckets,
        std::string pattern
    ){
        std::vector<std::string> carrying;
        carrying.push_back(std::move(pattern));
        std::size_t level = 0;
        while(level < buckets.size() && !buckets[level].patterns.empty()){
            auto& current = buckets[level].patterns;
            carrying.insert(
                carrying.end(),
                std::make_move_iterator(current.begin()),
                std::make_move_iterator(current.end())
            );
            buckets[level] = Bucket{};
            ++level;
        }
        if(level == buckets.size()) buckets.emplace_back();
        buckets[level].patterns = std::move(carrying);
        buckets[level].automaton =
            StaticAutomaton(buckets[level].patterns);
    }

    static long long count_buckets(
        const std::vector<Bucket>& buckets,
        std::string_view text
    ){
        long long result = 0;
        for(const Bucket& bucket: buckets){
            if(!bucket.patterns.empty()){
                result += bucket.automaton.count_matches(text);
            }
        }
        return result;
    }

public:
    void insert(std::string pattern){
        ++multiplicity_[pattern];
        insert_bucket(positive_, std::move(pattern));
    }

    void erase(const std::string& pattern){
        const auto iterator = multiplicity_.find(pattern);
        if(iterator == multiplicity_.end() || iterator->second == 0)
            [[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: missing pattern "
                "(DynamicAhoCorasick::erase)."
            );
        }
        if(--iterator->second == 0) multiplicity_.erase(iterator);
        insert_bucket(negative_, pattern);
    }

    int pattern_count(const std::string& pattern) const{
        const auto iterator = multiplicity_.find(pattern);
        return iterator == multiplicity_.end() ? 0 : iterator->second;
    }

    long long count_matches(std::string_view text) const{
        return count_buckets(positive_, text)
            - count_buckets(negative_, text);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_DYNAMIC_AHO_CORASICK_HPP_INCLUDED
