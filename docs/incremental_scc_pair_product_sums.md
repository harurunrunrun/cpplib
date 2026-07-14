---
title: Incremental SCC Pair-Product Sums (増分強連結成分内の重み積和) [incremental_scc]
documentation_of: ../src/algorithm/graph/incremental_scc_pair_product_sums.hpp
---

有向辺を順に追加した各時点について、同じ強連結成分に属する相異なる2頂点の
重み積を加算した値を返す。

# `incremental_scc_pair_product_sums`

```cpp
template<class T>
vector<T> incremental_scc_pair_product_sums(
    const vector<T>& vertex_weights,
    const vector<pair<int, int>>& added_edges
)
```

`added_edges[t]` までを追加したグラフの強連結成分を $C$ として、返り値の
`result[t]` は

$$
\sum_C\sum_{u<v,\ u,v\in C} w_u w_v
$$

である。辺が0本なら空列を返す。自己loopと多重辺を許す。

`T` はdefault construction、copy、`+=`、`*`を持ち、加算と乗算がこの式の
可換環演算として振る舞う必要がある。剰余型を渡せば全演算はその型の法で行われる。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N+M)\log(M+1))$。

## 空間計算量

$O(N+M)$。

## 注意点

辺端点は `[0, vertex_weights.size())` でなければならず、違反時は
`runtime_error`。頂点数または辺数が `INT_MAX` 以上なら `length_error`。
`T` の演算におけるoverflowや例外は型自身の契約に従う。
