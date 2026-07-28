---
title: Minimum-Cost Circulation (最小費用循環)
documentation_of: ../../../../src/algorithm/matching/flow/minimum_cost_circulation.hpp
---

各辺の下限・上限を守る循環のうち総費用最小のものを求める。
実行可能流を構築した後、負費用閉路にも流せる整数 cost-scaling 法で最適化する。

## API

```cpp
struct MinimumCostCirculationEdge {
    int from, to;
    long long lower, upper, cost;
};

struct MinimumCostCirculationResult {
    __int128 cost;
    vector<long long> flow;
};
```

`flow[i]` は入力辺 $i$ の流量、`cost` は
$\sum_i flow[i]\,cost[i]$ である。

```cpp
optional<MinimumCostCirculationResult> minimum_cost_circulation(
    int vertex_count,
    const vector<MinimumCostCirculationEdge>& edges
);
```

全頂点で流入量と流出量が等しい実行可能流がなければ `nullopt` を返す。
辺が空のグラフや頂点数 $0$ のグラフでは、費用 $0$ の空の循環を返す。

## 時間計算量

頂点数を $V$、辺数を $E$、費用絶対値の最大値を $C$ とする。

- 下限制約を除去する処理: $O(V+E)$
- Dinic 法による実行可能流構築: $O(V^2E)$
- 整数 cost-scaling による費用最適化:
  $O(VE\log(VC))$ 回の基本操作
- 追加空間: $O(V+E)$

## 注意点

`vertex_count` は非負、端点は $[0,V)$、各辺は
$0\le lower\le upper$ でなければならない。違反時は `runtime_error` を送出する。
需要総量などが `long long` を超える場合は `overflow_error`、残余辺数が内部の `int` 添字を超える場合は `length_error` を送出する。
流量と容量は `long long`、費用合計と内部の縮約費用は `__int128` で保持する。
