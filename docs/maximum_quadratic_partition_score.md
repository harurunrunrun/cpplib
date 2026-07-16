---
title: Maximum Quadratic Partition Score (二次評価による連続区間分割の最大得点) [APIO10A]
documentation_of: ../src/algorithm/other/maximum_quadratic_partition_score.hpp
---

正の値からなる列を空でない連続区間へ分割し、各区間和に同じ二次式を適用した得点の総和を最大化する。

## maximum_quadratic_partition_score

```cpp
long long maximum_quadratic_partition_score(
    const vector<long long>& segment_values,
    long long quadratic_coefficient,
    long long linear_coefficient,
    long long constant_coefficient
);
```

各区間和を $x$ とすると、区間の得点は $a x^2+b x+c$ である。$a$ は `quadratic_coefficient`、$b$ は `linear_coefficient`、$c$ は `constant_coefficient` に対応する。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_quadratic_partition_score` | $O(N)$ | $O(N)$ |

## 注意点

- `quadratic_coefficient` は負、`segment_values` の各要素は正でなければならない。この単調性を利用して凸包を線形時間で走査する。
- 接頭辞和または返り値が `long long` に収まらない場合は `overflow_error` を送出する。
- 前提違反は `runtime_error` を送出する。
