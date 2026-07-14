---
title: Minimum Temporary-Staff Cost (臨時雇用者の最小費用) [MKBUDGET]
documentation_of: ../src/algorithm/other/minimum_temporary_staff_cost.hpp
---

月ごとの必要人数を満たす雇用計画の最小費用を返す。

# minimum_temporary_staff_cost

```cpp
long long minimum_temporary_staff_cost(
    const vector<int>& required_workers,
    long long hiring_cost,
    long long monthly_salary,
    long long severance_cost
);
```

開始前の雇用者数を0人として、各月に `required_workers[month]` 人以上を雇用するための最小費用を返す。雇用時に `hiring_cost`、雇用中の各月に `monthly_salary`、解雇時に `severance_cost` が1人ごとに掛かる。最終月より後の解雇費用は加算しない。月数が0なら0を返す。

## 時間計算量

$M=\lvert\mathrm{required\_workers}\rvert$、$R=\max(\mathrm{required\_workers})$ とする。

- 入力検査: $O(M)$
- `minimum_temporary_staff_cost`: $O(M(R+1))$

## 空間計算量

- 動的計画法の作業領域: $O(R+1)$

## 注意点

- 必要人数と3種類の費用は非負でなければならない。違反時は `runtime_error` を送出する。
- 答えが `long long` に収まらない場合は `overflow_error` を送出する。
