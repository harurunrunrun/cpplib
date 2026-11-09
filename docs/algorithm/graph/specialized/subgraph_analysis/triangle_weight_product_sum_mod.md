---
title: Triangle Weight Product Sum Modulo (三角形頂点重み積和) [enumerate_triangles]
documentation_of: ../../../../../src/algorithm/graph/specialized/subgraph_analysis/triangle_weight_product_sum_mod.hpp
---

頂点重み付きの単純無向グラフの全三角形について、3 頂点の重みの積の総和を求める。

## `triangle_weight_product_sum_mod`

```cpp
long long triangle_weight_product_sum_mod(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    const vector<long long>& weights,
    long long modulus
)
```

各三角形 $(a,b,c)$ に対する `weights[a] * weights[b] * weights[c]` の総和を、`modulus` で割った余りとして返す。負の重みも非負の剰余に正規化して扱う。積は 128 bit 整数で計算するため、`modulus` は `long long` の範囲で任意の正の値をとれる。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O(N+M\sqrt{M})$。

## 空間計算量

正規化した重みと三角形列挙用の領域を含めて $O(N+M)$。

## 注意点

- `vertex_count` は非負、`weights.size()` は `vertex_count` と等しく、`modulus` は正でなければならない。
- 頂点番号は $[0,N)$ でなければならず、自己ループを含めてはならない。
- 上記の前提違反時は `runtime_error` を送出する。
- 多重辺は検査しない。入力が多重辺を含まない単純無向グラフであることを呼び出し側が保証しなければならない。
