---
title: Minimum-Cost Circulation (最小費用循環)
documentation_of: ../../../../src/algorithm/matching/flow/minimum_cost_circulation.hpp
---

各辺の下限・上限を守る循環のうち総費用最小のものを求める。負費用閉路にも流せる。

## API

```cpp
optional<MinimumCostBFlowResult> minimum_cost_circulation(
    int vertex_count,
    const vector<MinimumCostCirculationEdge>& edges
);
```

各辺は `from`、`to`、`lower`、`upper`、`cost` を持つ。実行可能でなければ `nullopt` を返す。

## 時間計算量

`MinimumCostBFlow::solve` と同じで、実行可能流構築が $O(V^2E)$、費用最適化が整数 cost-scaling 法による $O(VE\log(VC))$ 回の基本操作。

## 空間計算量

$O(V+E)$。

## 注意点

総費用は `__int128` で返す。入力条件と例外は `MinimumCostBFlow` と同じである。
