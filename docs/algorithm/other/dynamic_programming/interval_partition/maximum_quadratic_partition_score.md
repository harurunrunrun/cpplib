---
title: Maximum Quadratic Partition Score (二次評価による連続区間分割の最大得点) [APIO10A]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/maximum_quadratic_partition_score.hpp
---

正の値からなる列を空でない連続区間に分割し、各区間の和に同じ二次式を適用した得点の総和を最大化する。

## maximum_quadratic_partition_score

```cpp
long long maximum_quadratic_partition_score(
    const std::vector<long long>& segment_values,
    long long quadratic_coefficient,
    long long linear_coefficient,
    long long constant_coefficient
);
```

区間の和を $x$ とすると、その区間の得点は $a x^2+b x+c$ である。$a$ は `quadratic_coefficient`、$b$ は `linear_coefficient`、$c$ は `constant_coefficient` に対応する。空の列では $0$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_quadratic_partition_score` | $O(N)$ | $O(N)$ |

中間値は多倍長整数 `ExactInteger` で計算するため、定数倍は大きい。

## 注意点

- `quadratic_coefficient` は負、`segment_values` の各要素は正でなければならない。この単調性を利用して、凸包を線形時間で走査する（Convex Hull Trick）。
- 接頭辞和または戻り値が `long long` に収まらない場合は `std::overflow_error` を送出する。
- 上記の前提に違反した場合は `std::runtime_error` を送出する。
