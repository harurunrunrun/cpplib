---
title: Maximum Density Subgraph (最大密度部分グラフ)
documentation_of: ../../../../../src/algorithm/graph/specialized/subgraph_analysis/maximum_density_subgraph.hpp
---

無向グラフの空でない頂点部分集合 $S$ について、誘導辺数 $|E(S)|$ を頂点数 $|S|$ で割った密度が最大の集合を求める。

## API

```cpp
optional<MaximumDensitySubgraphResult> maximum_density_subgraph(
    int vertex_count,
    vector<pair<int, int>> edges
)
```

頂点が存在しない場合は `nullopt` を返す。多重辺は1本へまとめ、自己ループは密度に数えない。

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
- `vertices`: 最大密度を達成する頂点集合の1つ。昇順

## 時間計算量

頂点数を $N$、単純化後の辺数を $M$、Dinkelbach反復回数を $I$ とする。

- 辺の正規化: $O(M\log(M+1))$
- 1反復の最大閉包問題: $O((N+M)^2M)$。Dinic法の一般容量上界
- 全体: $O(M\log(M+1)+I(N+M)^2M)$

密度は分子が $[0,M]$、分母が $[1,N]$ の分数で、反復ごとに厳密に増えるため $I\le MN+1$。浮動小数の二分探索を使わず、有限回で厳密値へ到達する。

## 空間計算量

$O(N+M)$。

## 注意点

辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。辺がない非空グラフでは頂点 `0` だけの集合と密度0を返す。最大密度を達成する集合は一意とは限らない。
