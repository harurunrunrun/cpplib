---
title: Triangle Weight Product Sum Modulo (三角形頂点重み積和) [enumerate_triangles]
documentation_of: ../src/algorithm/graph/specialized/triangle_weight_product_sum_mod.hpp
---

頂点重み付き無向単純グラフの全三角形について、3頂点の重みの積を加算する。

## `triangle_weight_product_sum_mod`

```cpp
long long triangle_weight_product_sum_mod(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    const vector<long long>& weights,
    long long modulus
)
```

各三角形 $(a,b,c)$ に対する
`weights[a] * weights[b] * weights[c]` の総和を `modulus` で割った余りとして
返す。負の重みも非負の剰余へ正規化する。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O(N+M\sqrt M)$。

## 空間計算量

正規化済み重みと三角形列挙用領域を含めて $O(N+M)$。

## 注意点

- `vertex_count` は非負、`weights.size()` は `vertex_count` と等しく、
  `modulus` は正でなければならない。
- 頂点番号は $[0,N)$ でなければならず、自己loopを含めてはならない。
- 上記の前提違反時は `runtime_error` を送出する。
- 多重辺は検査しない。入力が多重辺を含まない無向単純グラフであることを
  呼出側が保証しなければならない。
