---
title: Rooted Spanning Forest Count (重み付き根付き全域森の数え上げ)
documentation_of: ../../../../src/algorithm/graph/spanning_tree/rooted_spanning_forest_count.hpp
---

各木で根をちょうど 1 個選んだ全域森すべてについて、選んだ辺の重みと根の重みの積を合計する。行列森林定理により $\det(L + \mathrm{diag}(r))$ として計算する（$L$ は重み付きラプラシアン、$r$ は根の重みの列）。

## API

```cpp
struct RootedForestEdge { int first, second; long long weight; };

long long count_rooted_spanning_forests_mod(
    const std::vector<long long>& root_weights,
    const std::vector<RootedForestEdge>& edges,
    long long modulus
)

template <int MOD = 998244353, int MAX_SIZE = -1>
Modint<MOD> count_rooted_spanning_forests_prime(
    const std::vector<long long>& root_weights,
    const std::vector<RootedForestEdge>& edges,
    std::uint64_t seed = std::random_device{}()
)
```

- `count_rooted_spanning_forests_mod` は、合成数を含む任意の法 `modulus` での値を、互除法による消去で行列式を求めて返す。
- `count_rooted_spanning_forests_prime` は、素数の法 `MOD` での値を、疎行列の行列式を求めるモンテカルロ法で返す。`MAX_SIZE` を省略（`-1`）すると実行時のサイズに合わせた `std::vector` を使い、指定すると容量 `MAX_SIZE` の固定長版になる。

```cpp
auto value = count_rooted_spanning_forests_prime<998244353>(roots, edges, seed);
auto fixed_value = count_rooted_spanning_forests_prime<998244353, 100000>(roots, edges, seed);
```

頂点数は `root_weights` の長さである。多重辺と負の重みを許し、自己ループは無視する。

## 計算量

頂点数を $N$、辺数を $E$、法を $m$ とする。時間計算量と空間計算量は次のとおりである。

- `count_rooted_spanning_forests_mod`: 時間 $O(N^3 \log m + E)$、空間 $O(N^2)$
- `count_rooted_spanning_forests_prime`: 時間 $O(N(E + N))$、空間は可変長版で $O(N + E)$、固定長版で $O(N + E + \texttt{MAX_SIZE})$

## 注意点

- 合成数版の法は $1 \le m \le 2^{31} - 1$（`INT_MAX`）でなければならない。違反時は `invalid_argument` を送出する。
- 辺の端点が範囲外なら `out_of_range` を送出する。素数版で頂点数が `MAX_SIZE` を超える場合は `length_error` を送出する。
- 素数版はモンテカルロ法を使うため、誤った値を返す確率がわずかに残る。
- 頂点のない空グラフでは $1$ を返す。頂点が存在し、根の重みがすべて $0$ なら $0$ を返す。
