---
title: Minimum-Cost Circulation (最小費用循環)
documentation_of: ../../../../src/algorithm/matching/flow/minimum_cost_circulation.hpp
---

各辺の流量の下限・上限を守る循環フローのうち、総コストが最小のものを求める。実行可能フローを構築した後、負コストの閉路にも流せる整数コストスケーリング法で最適化する。

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

`flow[i]` は入力辺 $i$ の流量であり、`cost` は $\sum_i f_i c_i$（$f_i$ は `flow[i]`、$c_i$ は辺 $i$ の `cost`）である。

```cpp
optional<MinimumCostCirculationResult> minimum_cost_circulation(
    int vertex_count,
    const vector<MinimumCostCirculationEdge>& edges
);
```

全頂点で流入量と流出量が等しい実行可能なフローがなければ `nullopt` を返す。辺が空のグラフや頂点数 $0$ のグラフでは、コスト $0$ の空の循環を返す。

## 時間計算量

頂点数を $V$、辺数を $E$、コストの絶対値の最大値を $C$ とする。

- 下限制約を除去する処理: $O(V+E)$
- Dinic 法による実行可能フローの構築: $O(V^2 E)$
- 整数コストスケーリングによるコストの最適化: $O(VE\log((V+1)C+1))$ 回の基本操作

## 空間計算量

$O(V+E)$。

## 注意点

`vertex_count` は非負、端点は $[0, V)$ に含まれ、各辺は $0 \le \mathit{lower} \le \mathit{upper}$ を満たさなければならない。違反時は `runtime_error` を送出する。需要の総量などが `long long` を超える場合や頂点数が大きすぎる場合は `overflow_error` を、残余辺の数が内部の `int` 添字の範囲を超える場合は `length_error` を送出する。流量と容量は `long long`、コストの合計と内部の縮約コストは `__int128` で保持する。
