---
title: Minimum Mean Cycle (最小平均重み閉路)
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/minimum_mean_cycle.hpp
---

重み付き有向グラフに含まれる閉路のうち、1 辺あたりの平均重みが最小のものについて、その平均重みを求める。

## API

```cpp
struct MinimumMeanCycleEdge {
    int from;
    int to;
    long long weight;
};
```

有向辺とその重みを表す。重みは負でもよい。

```cpp
optional<MinimumMeanCycleResult> minimum_mean_cycle(
    int vertex_count,
    const vector<MinimumMeanCycleEdge>& edges
)
```

```cpp
struct MinimumMeanCycleResult {
    long long numerator;
    int denominator;
    long double mean;
};
```

- `numerator / denominator`: 最小平均重みを表す既約分数（`denominator` は正）
- `mean`: 同じ値の `long double` 表現

閉路が存在しない場合（頂点数が $0$ の場合を含む）は `nullopt` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- Karp 法の動的計画法: $O(NM)$
- 最小平均値の分数比較: $O(N^2)$
- `minimum_mean_cycle` 全体: $O(NM+N^2)$

## 空間計算量

$O(N^2)$。

## 注意点

- 辺の端点は $[0,N)$ でなければならない。頂点数が負の場合も含め、違反時は `runtime_error` を送出する。
- 内部計算には符号付き 128 bit 整数を使い、約分後の分子を `long long` で表せない場合は `overflow_error` を送出する。
