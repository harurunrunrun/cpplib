---
title: Connected Component Value Sums (連結成分頂点値和) [IAPCR2F]
documentation_of: ../../../../../src/algorithm/graph/connectivity/components/connected_component_value_sums.hpp
---

無向グラフの各連結成分について、属する頂点の値の総和を求める。

## connected_component_value_sums

```cpp
vector<long long> connected_component_value_sums(
    const vector<long long>& values,
    const vector<pair<int, int>>& edges
)
```

## 引数

- `values[v]`: 頂点 `v` の値。頂点数 $N$ は `values.size()` である。
- `edges`: 0-indexed の端点を持つ無向辺の列。

## 戻り値

各連結成分の頂点値の総和を、昇順に並べて返す（孤立頂点も 1 つの成分として数える）。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、成分数を $K$、逆アッカーマン関数を $\alpha$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `connected_component_value_sums` | $O((N + M) \alpha(N) + K \log (K + 1))$ | $O(N)$ |

## 注意点

- 辺の端点は $[0, N)$ でなければならず、違反時は `std::out_of_range` を送出する。
- 成分和が `long long` の範囲を超える場合は `std::overflow_error` を送出する。
- `values.size()` が `int` の最大値を超える場合は `std::length_error` を送出する。
- 自己ループと多重辺を許す。
