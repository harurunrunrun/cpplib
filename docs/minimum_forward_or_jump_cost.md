---
title: Minimum Forward or Jump Cost (前進または跳躍の最小費用) [ABC340_D]
documentation_of: ../src/algorithm/graph/minimum_forward_or_jump_cost.hpp
---

各頂点から次頂点への前進辺と任意頂点への跳躍辺を1本ずつ持つグラフの最小費用を
求める。

# `minimum_forward_or_jump_cost`

```cpp
template<class Cost>
Cost minimum_forward_or_jump_cost(
    const vector<Cost>& forward_cost,
    const vector<Cost>& jump_cost,
    const vector<int>& jump_target
)
```

`N = forward_cost.size() + 1` 頂点を持つ有向グラフを考える。各
`0 <= v < N-1` からは、費用 `forward_cost[v]` の `v+1` への辺と、費用
`jump_cost[v]` の `jump_target[v]` への辺がある。頂点0から頂点`N-1`までの
最小費用を返す。3列が空なら0を返す。

## 時間計算量

`N = forward_cost.size() + 1` として $O(N\log(N+1))$。

## 空間計算量

構築するグラフと探索領域を含めて $O(N)$。

## 注意点

- 3列の長さは等しくなければならない。
- 費用は非負で、各 `jump_target[v]` は `0` 以上 `N` 未満でなければならない。
- `Cost` はdefault構築、比較、加算を提供し、経路費用をoverflowせず表現できる
  必要がある。
- 前提違反では `runtime_error` を送出する。
