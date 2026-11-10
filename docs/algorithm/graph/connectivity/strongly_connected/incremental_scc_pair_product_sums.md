---
title: Incremental SCC Pair-Product Sums (増分強連結成分内の重み積和) [incremental_scc]
documentation_of: ../../../../../src/algorithm/graph/connectivity/strongly_connected/incremental_scc_pair_product_sums.hpp
---

有向辺を順に追加していく各時点について、同じ強連結成分に属する相異なる 2 頂点の重みの積の総和を返す。`incremental_scc` で求めたマージ時刻に従って DSU で成分を併合し、併合のたびに 2 成分の重みの和の積を加える。

## `incremental_scc_pair_product_sums`

```cpp
template<class T>
vector<T> incremental_scc_pair_product_sums(
    const vector<T>& vertex_weights,
    const vector<pair<int, int>>& added_edges
)
```

`added_edges[0]` から `added_edges[t]` までを追加したグラフの強連結成分を $C$ として、戻り値の `result[t]` は

$$
\sum_C \sum_{u < v,\ u, v \in C} w_u w_v
$$

である。辺が 0 本なら空列を返す。自己ループと多重辺を許す。

`T` はデフォルト構築（`T{}` が加法単位元 0 になること）、コピー、`+=`、`*` を持ち、加算と乗算がこの式の可換環の演算として振る舞う必要がある。剰余型を渡せば、全演算はその型の法のもとで行われる。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N + M) \log (M + 1))$。

## 空間計算量

$O(N + M)$。

## 注意点

- 辺の端点は $[0, N)$（$N$ は `vertex_weights.size()`）でなければならず、違反時は `std::runtime_error` を送出する。
- 頂点数または辺数が `INT_MAX` 以上なら `std::length_error` を送出する。
- `T` の演算におけるオーバーフローや例外は、型自身の仕様に従う。
