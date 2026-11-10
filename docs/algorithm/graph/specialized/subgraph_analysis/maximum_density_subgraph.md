---
title: Maximum Density Subgraph (最大密度部分グラフ)
documentation_of: ../../../../../src/algorithm/graph/specialized/subgraph_analysis/maximum_density_subgraph.hpp
---

無向グラフの空でない頂点部分集合 $S$ のうち、誘導される辺の本数 $\lvert E(S)\rvert$ を頂点数 $\lvert S\rvert$ で割った密度が最大となるものを求める。

## API

```cpp
optional<MaximumDensitySubgraphResult> maximum_density_subgraph(
    int vertex_count,
    vector<pair<int, int>> edges
)
```

頂点が存在しない場合は `nullopt` を返す。多重辺は 1 本にまとめ、自己ループは密度に数えない。

```cpp
struct MaximumDensitySubgraphResult {
    long long numerator;
    int denominator;
    long double density;
    vector<int> vertices;
};
```

- `numerator / denominator`: 最大密度を表す既約分数
- `density`: 同じ値の `long double` 表現
- `vertices`: 最大密度を達成する頂点集合の 1 つ（昇順）

## 時間計算量

頂点数を $N$、単純化後の辺数を $M$、Dinkelbach 法の反復回数を $I$ とする。

- 辺の正規化: $O(M\log(M+1))$
- 1 回の反復での最大閉包問題: 頂点数・辺数がともに $O(N+M)$ のネットワーク上の Dinic 法で、一般容量の上界により $O((N+M)^3)$
- 全体: $O(M\log(M+1)+I(N+M)^3)$

密度は分子が $[0,M]$、分母が $[1,N]$ の分数で、反復ごとに真に増加するため $I\le MN+1$ である。浮動小数点数の二分探索を使わず、有限回の反復で厳密値に到達する。

## 空間計算量

$O(N+M)$。

## 注意点

- 頂点数は非負、辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
- 辺がない（自己ループのみの場合を含む）非空グラフでは、頂点 `0` だけの集合と密度 $0$ を返す。
- 最大密度を達成する集合は一意とは限らない。
