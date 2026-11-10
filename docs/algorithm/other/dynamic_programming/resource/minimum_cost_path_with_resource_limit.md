---
title: Minimum-Cost Path with Resource Limit (資源上限制約付き最小費用経路) [FISHER]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/resource/minimum_cost_path_with_resource_limit.hpp
---

各辺が資源の使用量と費用をもつ有向グラフで、始点から終点への歩道（同じ頂点や辺を何度通ってもよい経路）のうち、資源の使用量の合計が上限以下で費用が最小のものを求める。使用した資源量ごとに頂点への最小費用を持つ DP で計算する。

## ResourceConstrainedEdge

```cpp
struct ResourceConstrainedEdge {
    int to;
    int resource_usage;
    long long cost;
};
```

有向辺の終点、資源の使用量（正）、費用（非負）を表す。

## ResourceConstrainedPathResult

```cpp
struct ResourceConstrainedPathResult {
    long long cost;
    int resource_used;
};
```

最小の費用と、その経路が使用した資源量を表す。最小費用を達成する経路が複数ある場合は、資源の使用量が最小のものを返す。

## minimum_cost_path_with_resource_limit

```cpp
std::optional<ResourceConstrainedPathResult>
minimum_cost_path_with_resource_limit(
    const std::vector<std::vector<ResourceConstrainedEdge>>& graph,
    int source,
    int target,
    int resource_limit
);
```

到達可能なら結果を返し、資源の上限内で到達できなければ `std::nullopt` を返す。`source == target` なら費用・資源使用量ともに $0$ である。

## API別の時間計算量・空間計算量

$V$ を頂点数、$E$ を辺数、$R$ を `resource_limit` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_cost_path_with_resource_limit` | $O(R(V+E))$ | $O(RV)$ |

## 注意点

- `graph` は空であってはならず、`source`、`target`、各辺の `to` は頂点範囲内でなければならない。
- `resource_limit` は非負、各辺の `resource_usage` は正、`cost` は非負でなければならない。資源の使用量が正なので、使用した資源量の小さい順に状態を処理できる。
- 前提に違反した場合は `std::runtime_error`、最小費用が `long long` に収まらない場合は `std::overflow_error` を送出する。
