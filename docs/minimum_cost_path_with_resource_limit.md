---
title: Minimum-Cost Path with Resource Limit (資源上限制約付き最小費用経路) [FISHER]
documentation_of: ../src/algorithm/other/dynamic_programming/minimum_cost_path_with_resource_limit.hpp
---

各辺が資源使用量と費用を持つ有向グラフで、資源使用量の合計が上限以下となる始点から終点へのwalkのうち、費用が最小のものを求める。

## ResourceConstrainedEdge

```cpp
struct ResourceConstrainedEdge {
    int to;
    int resource_usage;
    long long cost;
};
```

有向辺の終点、正の資源使用量、非負の費用を表す。

## ResourceConstrainedPathResult

```cpp
struct ResourceConstrainedPathResult {
    long long cost;
    int resource_used;
};
```

最小費用と、その経路が使用した資源量を表す。同じ最小費用を持つ経路が複数ある場合は、資源使用量が最小のものを返す。

## minimum_cost_path_with_resource_limit

```cpp
optional<ResourceConstrainedPathResult>
minimum_cost_path_with_resource_limit(
    const vector<vector<ResourceConstrainedEdge>>& graph,
    int source,
    int target,
    int resource_limit
);
```

到達可能なら結果を返し、資源上限内で到達できなければ `nullopt` を返す。`source == target` なら費用・資源使用量ともに0である。

## API別の時間計算量・空間計算量

$V$ を頂点数、$E$ を辺数、$R=mathtt{resource\_limit}$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_cost_path_with_resource_limit` | $O(R(V+E))$ | $O(RV)$ |

## 注意点

- `graph` は空であってはならず、`source`、`target`、各辺の `to` は頂点範囲内でなければならない。
- `resource_limit` は非負、各辺の `resource_usage` は正、`cost` は非負でなければならない。資源使用量が正なので、資源量を増加順に処理できる。
- 前提違反は `runtime_error`、最小費用が `long long` に収まらない場合は `overflow_error` を送出する。
