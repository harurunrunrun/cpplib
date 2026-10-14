---
title: Rooted Spanning Forest Count (重み付き根付き全域森の数え上げ)
documentation_of: ../../../../src/algorithm/graph/spanning_tree/rooted_spanning_forest_count.hpp
---

各木で根をちょうど1個選んだ全域森について、選んだ辺重みと根重みの積を合計する。`count_rooted_spanning_forests_mod(root_weights,edges,modulus)` は合成数法に、`count_rooted_spanning_forests_prime<MOD,MAX_SIZE>(root_weights,edges,seed)` は素数法に対応する。辺は `RootedForestEdge{first,second,weight}`。多重辺と負の重みを許し、自己ループは無視する。行列森林定理の $\det(L+\mathrm{diag}(r))$ を使う。

## 時間計算量

合成数版は $O(N^3\log m+E)$、空間 $O(N^2)$。素数版は $O(N(E+N))$、空間 $O(N+E+\mathrm{MAX\_SIZE})$。

## 注意点

合成数版の法は $1\le m\le\mathrm{INT\_MAX}$。素数版は疎行列式のMonte Carlo法を使うため、誤判定確率が残る。空グラフの返り値は1。根重みがすべて0で頂点が存在すれば0。

## 容量を省略する可変長版

容量テンプレート引数をすべて省略すると、実行時サイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
auto value = count_rooted_spanning_forests_prime<998244353>(roots, edges, seed);
auto fixed_value = count_rooted_spanning_forests_prime<998244353, 100000>(roots, edges, seed);
```

素数法の時間は $O(N(E+N))$、空間は $O(N+E)$。合成数法の API と計算量は変わらない。
