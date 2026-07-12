// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/structure/array/persistent_array.hpp"
#include "../../src/structure/convex_hull_trick/persistent_convex_hull_trick.hpp"
#include "../../src/structure/convex_hull_trick/persistent_dynamic_li_chao_tree.hpp"
#include "../../src/structure/dsu/persistent_dsu.hpp"
#include "../../src/structure/trie/persistent_binary_trie.hpp"
#include "../../src/structure/trie/persistent_trie.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_functional_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d_weighted.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_weighted_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d.hpp"
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d_weighted.hpp"
#include "../../src/structure/wavelet_matrix/persistent_weighted_wavelet_matrix.hpp"

template<class F>
bool throws_runtime_error(F&& operation){
    try{
        operation();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

struct ThrowingValue{
    int value = 0;
    static inline bool throw_on_copy = false;

    ThrowingValue() = default;
    explicit ThrowingValue(int x): value(x){}
    ThrowingValue(const ThrowingValue& other): value(other.value){
        if(throw_on_copy) throw std::runtime_error("copy failure");
    }
    ThrowingValue& operator=(const ThrowingValue&) = default;
};

int main(){
    int q;
    if(!(std::cin >> q)) return 0;
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "TRIE_NODE"){
            PersistentTrie<4, 7, 4> trie;
            bool thrown = throws_runtime_error([&]{ trie.insert(0, "aaaaaa"); });
            int version = trie.insert(0, "a");
            std::cout << thrown << ' ' << trie.versions() << ' '
                      << trie.node_count() << ' ' << trie.count(0, "a") << ' '
                      << trie.count(version, "a") << '\n';
        }else if(type == "TRIE_VERSION"){
            PersistentTrie<4, 16, 1> trie;
            int nodes = trie.node_count();
            bool thrown = throws_runtime_error([&]{ trie.insert(0, "abc"); });
            std::cout << thrown << ' ' << trie.versions() << ' '
                      << (trie.node_count() == nodes) << ' ' << trie.size(0) << '\n';
        }else if(type == "TRIE_ERASE_NODE"){
            PersistentTrie<4, 5, 4> trie;
            int version = trie.insert(0, "a");
            int nodes = trie.node_count();
            bool thrown = throws_runtime_error([&]{ trie.erase(version, "a"); });
            std::cout << thrown << ' ' << trie.versions() << ' '
                      << (trie.node_count() == nodes) << ' '
                      << trie.count(version, "a") << '\n';
        }else if(type == "TRIE_INVALID"){
            PersistentTrie<2, 16, 4> trie;
            int nodes = trie.node_count();
            bool insert_thrown = throws_runtime_error([&]{ trie.insert(0, "a?"); });
            bool query_thrown = throws_runtime_error([&]{ trie.count(0, "a?"); });
            std::cout << insert_thrown << ' ' << query_thrown << ' '
                      << trie.versions() << ' ' << (trie.node_count() == nodes) << '\n';
        }else if(type == "BINARY_NODE"){
            PersistentBinaryTrie<unsigned, 4, 5, 4> trie;
            int nodes = trie.node_count();
            bool thrown = throws_runtime_error([&]{ trie.insert(0, 7); });
            std::cout << thrown << ' ' << trie.versions() << ' '
                      << (trie.node_count() == nodes) << ' ' << trie.size(0) << '\n';
        }else if(type == "BINARY_VERSION"){
            PersistentBinaryTrie<unsigned, 4, 16, 1> trie;
            int nodes = trie.node_count();
            bool thrown = throws_runtime_error([&]{ trie.insert(0, 7); });
            std::cout << thrown << ' ' << trie.versions() << ' '
                      << (trie.node_count() == nodes) << ' ' << trie.size(0) << '\n';
        }else if(type == "BINARY_ERASE_NODE"){
            PersistentBinaryTrie<unsigned, 4, 10, 4> trie;
            int version = trie.insert(0, 7);
            int nodes = trie.node_count();
            bool thrown = throws_runtime_error([&]{ trie.erase(version, 7); });
            std::cout << thrown << ' ' << trie.versions() << ' '
                      << (trie.node_count() == nodes) << ' '
                      << trie.count(version, 7) << '\n';
        }else if(type == "DSU_VERSION"){
            PersistentDSU<4, 1> dsu(4);
            int version = dsu.merge(0, 0, 1);
            int nodes = dsu.nodes_used();
            bool thrown = throws_runtime_error([&]{ dsu.merge(0, 2, 3); });
            std::cout << thrown << ' ' << dsu.versions() << ' '
                      << (dsu.nodes_used() == nodes) << ' '
                      << dsu.same(0, 0, 1) << ' ' << dsu.same(version, 0, 1) << '\n';
        }else if(type == "ARRAY_COPY"){
            PersistentArray<ThrowingValue, 4, 2> array(4, ThrowingValue(7));
            ThrowingValue value(9);
            ThrowingValue::throw_on_copy = true;
            bool thrown = throws_runtime_error([&]{ array.set(0, 2, value); });
            ThrowingValue::throw_on_copy = false;
            int version = array.set(0, 2, value);
            std::cout << thrown << ' ' << array.versions() << ' '
                      << array.get(0, 2).value << ' ' << array.get(version, 2).value << '\n';
        }else if(type == "CHT_NODE"){
            PersistentConvexHullTrick<4, 4, 2> cht;
            int nodes = cht.nodes_used();
            bool thrown = throws_runtime_error([&]{ cht.add_line(0, 1, 0); });
            std::cout << thrown << ' ' << cht.versions() << ' '
                      << (cht.nodes_used() == nodes) << ' ' << cht.empty(0) << '\n';
        }else if(type == "WAVELET_DEFAULT"){
            PersistentWaveletMatrix<int, 0, 0> matrix;
            bool thrown = throws_runtime_error([&]{ matrix.fork(0); });
            std::cout << thrown << ' ' << matrix.size() << ' '
                      << matrix.versions() << ' ' << matrix.latest_version() << '\n';
        }else if(type == "WEIGHTED_WAVELET_DEFAULT"){
            PersistentWeightedWaveletMatrix<int, long long, 0, 0> matrix;
            bool thrown = throws_runtime_error([&]{ matrix.fork(0); });
            std::cout << thrown << ' ' << matrix.size() << ' '
                      << matrix.versions() << ' ' << matrix.latest_version() << '\n';
        }else if(type == "WAVELET_2D_DEFAULT"){
            PersistentWaveletMatrix2D<int, int, 0, 0> matrix;
            bool thrown = throws_runtime_error([&]{ matrix.fork(0); });
            std::cout << thrown << ' ' << matrix.size() << ' '
                      << matrix.versions() << ' '
                      << matrix.rectangle_count(0, -1, 1, -1, 1) << '\n';
        }else if(type == "WEIGHTED_WAVELET_2D_DEFAULT"){
            PersistentWaveletMatrix2DWeighted<int, int, long long, 0, 0> matrix;
            bool thrown = throws_runtime_error([&]{ matrix.fork(0); });
            std::cout << thrown << ' ' << matrix.size() << ' '
                      << matrix.versions() << ' '
                      << matrix.rectangle_sum(0, -1, 1, -1, 1) << '\n';
        }else if(type == "PARTIAL_WAVELET_DEFAULT"){
            PartiallyPersistentWaveletMatrix<int, 0, 0> matrix;
            std::cout << matrix.size() << ' ' << matrix.versions() << ' '
                      << matrix.latest_version() << '\n';
        }else if(type == "PARTIAL_WEIGHTED_WAVELET_DEFAULT"){
            PartiallyPersistentWeightedWaveletMatrix<int, long long, 0, 0> matrix;
            std::cout << matrix.size() << ' ' << matrix.versions() << ' '
                      << matrix.latest_version() << '\n';
        }else if(type == "PARTIAL_FUNCTIONAL_WAVELET_DEFAULT"){
            PartiallyPersistentFunctionalWaveletMatrix<int, 0, 0> matrix;
            std::cout << matrix.size() << ' ' << matrix.versions() << ' '
                      << matrix.latest_version() << '\n';
        }else if(type == "PARTIAL_WAVELET_2D_DEFAULT"){
            PartiallyPersistentWaveletMatrix2D<int, int, 0, 0> matrix;
            std::cout << matrix.size() << ' ' << matrix.versions() << ' '
                      << matrix.rectangle_count(0, -1, 1, -1, 1) << '\n';
        }else if(type == "PARTIAL_WEIGHTED_WAVELET_2D_DEFAULT"){
            PartiallyPersistentWaveletMatrix2DWeighted<int, int, long long, 0, 0> matrix;
            std::cout << matrix.size() << ' ' << matrix.versions() << ' '
                      << matrix.rectangle_sum(0, -1, 1, -1, 1) << '\n';
        }else if(type == "LICHAO_VERSION"){
            PersistentDynamicLiChaoTree<-4, 4, 1> tree;
            int version = tree.add_line(0, 2, 3);
            int nodes = tree.nodes_used();
            bool thrown = throws_runtime_error([&]{ tree.add_line(0, -1, 4); });
            std::cout << thrown << ' ' << tree.versions() << ' '
                      << (tree.nodes_used() == nodes) << ' '
                      << tree.query(version, 2) << '\n';
        }
    }
}
