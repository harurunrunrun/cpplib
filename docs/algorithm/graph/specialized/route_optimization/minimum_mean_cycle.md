---
title: Minimum Mean Cycle (最小平均重み閉路)
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/minimum_mean_cycle.hpp
---

重み付き有向グラフに含まれる閉路のうち、1辺当たりの平均重みが最小のものの値を求める。

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

- `numerator / denominator`: 最小平均重みを表す既約分数
- `mean`: 同じ値の `long double` 表現

閉路が存在しない場合は `nullopt` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- Karp法の動的計画: $O(NM)$
- 最小平均値の分数比較: $O(N^2)$
- `minimum_mean_cycle` 全体: $O(NM+N^2)$

## 空間計算量

$O(N^2)$。

## 注意点

辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。内部計算には符号付き128 bit整数を使い、最終分子を `long long` で表せない場合は `overflow_error` を送出する。
