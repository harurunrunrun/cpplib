---
title: Minimum Forward or Jump Cost (前進または跳躍の最小費用) [ABC340_D]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/state_space/minimum_forward_or_jump_cost.hpp
---

各頂点から次の頂点への前進辺と、任意の頂点への跳躍辺を 1 本ずつもつグラフで、先頭から末尾への最小コストを Dijkstra 法で求める。

## `minimum_forward_or_jump_cost`

```cpp
template <class Cost>
Cost minimum_forward_or_jump_cost(
    const std::vector<Cost>& forward_cost,
    const std::vector<Cost>& jump_cost,
    const std::vector<int>& jump_target
)
```

$N$ = `forward_cost.size() + 1` 頂点の有向グラフを考える。各 $0 \le v < N-1$ からは、コスト `forward_cost[v]` の `v + 1` への辺と、コスト `jump_cost[v]` の `jump_target[v]` への辺がある。頂点 $0$ から頂点 $N-1$ までの最小コストを返す。3 つの列がすべて空なら $0$ を返す。

## 時間計算量

$O(N \log (N + 1))$

## 空間計算量

構築するグラフと探索領域を含めて $O(N)$。

## 注意点

- 3 つの列の長さは等しくなければならない。
- コストは非負で、各 `jump_target[v]` は $0$ 以上 $N$ 未満でなければならない。
- `Cost` はデフォルト構築（値 $0$ を表す）、比較、加算ができ、経路のコストをオーバーフローせずに表現できなければならない。
- これらの前提に違反すると `runtime_error` を送出する。
