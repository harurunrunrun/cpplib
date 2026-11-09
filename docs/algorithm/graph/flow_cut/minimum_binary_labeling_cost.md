---
title: Minimum Binary Labeling Cost (二値ラベル付け最小コスト) [COCONUTS]
documentation_of: ../../../../src/algorithm/graph/flow_cut/minimum_binary_labeling_cost.hpp
---

各変数に 0 または 1 を割り当て、各変数のラベルごとの単項コストと、指定された変数対のラベルが異なるときのコストの総和を最小化する。劣モジュラな二値エネルギーの基本形を $s$-$t$ 最小カットに帰着して求める。

## `BinaryLabelUnaryCost`

```cpp
struct BinaryLabelUnaryCost {
    long long cost_if_zero;
    long long cost_if_one;
};
```

1 つの変数を 0 または 1 にしたときの単項コストを表す。

## `BinaryLabelPairwiseCost`

```cpp
struct BinaryLabelPairwiseCost {
    int left;
    int right;
    long long different_cost;
};
```

`left` と `right` のラベルが異なる場合に `different_cost` を加える。両端に同じ変数を指定した項は常に 0 なので無視する。同じ変数対を複数回指定した場合はすべて加算する。

## `minimum_binary_labeling_cost`

```cpp
long long minimum_binary_labeling_cost(
    const std::vector<BinaryLabelUnaryCost>& unary_costs,
    const std::vector<BinaryLabelPairwiseCost>& pairwise_costs
);
```

すべての二値ラベル列 $x$ についての次の値の最小値を返す。

$$
\sum_v U_v(x_v) + \sum_{e = (u, v)} c_e \, [x_u \ne x_v]
$$

ここで $U_v(0)$ は `unary_costs[v].cost_if_zero`、$U_v(1)$ は `unary_costs[v].cost_if_one`、$c_e$ は二変数項 $e$ の `different_cost` である。

## 計算量

変数の個数を $N$、二変数項の個数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_binary_labeling_cost` | $O(N^2 (N + M))$ | $O(N + M)$ |

## 注意点

- すべてのコストは非負で、二変数項の各端点は $[0, N)$ の範囲内でなければならない。
- 負のコストには `invalid_argument`、範囲外の端点には `out_of_range` を送出する。
- 変数の個数が `INT_MAX - 2` を超える場合は `length_error` を送出する。
- 最小値が `long long` の範囲を超える場合は `overflow_error` を送出する。
- 内部の容量には符号付き 128 ビット整数を用いる。
