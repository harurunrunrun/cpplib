---
title: Minimum Binary Labeling Cost (二値ラベル付け最小コスト) [COCONUTS]
documentation_of: ../src/algorithm/graph/minimum_binary_labeling_cost.hpp
---

各変数に0または1を割り当てる。
各変数のラベル別単項コストと、指定された変数対のラベルが異なるときのコストの総和を最小化する。
劣モジュラな二値エネルギーの基本形を $s$-$t$ 最小cutへ帰着して求める。

## `BinaryLabelUnaryCost`

```cpp
struct BinaryLabelUnaryCost {
    long long cost_if_zero;
    long long cost_if_one;
};
```

1変数を0または1にしたときの単項コストを表す。

## `BinaryLabelPairwiseCost`

```cpp
struct BinaryLabelPairwiseCost {
    int left;
    int right;
    long long different_cost;
};
```

`left` と `right` のラベルが異なる場合に `different_cost` を加える。
同じ変数を両端に指定した項は常に0なので無視される。
同じ変数対を複数指定した場合はすべて加算される。

## `minimum_binary_labeling_cost`

```cpp
long long minimum_binary_labeling_cost(
    const vector<BinaryLabelUnaryCost>& unary_costs,
    const vector<BinaryLabelPairwiseCost>& pairwise_costs
);
```

次の値を全二値ラベル列 $x$ について最小化して返す。

$$
\sum_v U_v(x_v)
+\sum_{e=(u,v)} e.different\_cost\,[x_u\ne x_v]
$$

ここで $U_v(0)=unary\_costs[v].cost\_if\_zero$、
$U_v(1)=unary\_costs[v].cost\_if\_one$ とする。

## API別の時間計算量・空間計算量

変数数を $N$、二変数項数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_binary_labeling_cost` | $O(N^2(N+M))$ | $O(N+M)$ |

## 注意点

- すべてのコストは非負で、各端点は $[0,N)$ でなければならない。
- 負コストには `invalid_argument`、範囲外端点には `out_of_range` を送出する。
- 最小値が `long long` を越える場合は `overflow_error` を送出する。
- 内部容量には符号付き128 bit整数を用いる。
