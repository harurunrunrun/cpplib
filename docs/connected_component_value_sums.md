---
title: Connected Component Value Sums (連結成分頂点値和) [IAPCR2F]
documentation_of: ../src/algorithm/graph/connectivity/connected_component_value_sums.hpp
---

## connected_component_value_sums

```cpp
vector<long long> connected_component_value_sums(
    const vector<long long>& values,
    const vector<pair<int, int>>& edges
)
```

## 引数

- `values[v]`: 頂点 `v` の値。
- `edges`: 0-indexed端点を持つ無向辺列。

## 戻り値

各連結成分の頂点値和を昇順で返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、成分数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `connected_component_value_sums` | $O((N+M)\alpha(N)+K\log(K+1))$ | $O(N)$ |

## 注意点

- 辺端点は $[0,N)$ でなければならない。
- 端点違反では `out_of_range`、成分和のoverflowでは `overflow_error` を送出する。
