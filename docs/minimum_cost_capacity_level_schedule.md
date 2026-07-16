---
title: Minimum-Cost Capacity-Level Schedule (容量水準スケジュールの最小費用) [MKBUDGET]
documentation_of: ../src/algorithm/other/minimum_cost_capacity_level_schedule.hpp
---

期間ごとの最低容量を満たすように容量水準を選び、水準の増減費用と期間ごとの維持費用の総和を最小化する。

## minimum_cost_capacity_level_schedule

```cpp
long long minimum_cost_capacity_level_schedule(
    const vector<int>& minimum_required_levels,
    long long increase_cost_per_unit,
    long long level_cost_per_period,
    long long decrease_cost_per_unit
);
```

初期水準を0とする。各期間の水準は `minimum_required_levels[period]` 以上であり、前期間から1単位増やすごとに `increase_cost_per_unit`、減らすごとに `decrease_cost_per_unit`、その期間に水準を1維持するごとに `level_cost_per_period` を加算する。最終期間後に水準を0へ戻す費用は加算しない。

## API別の時間計算量・空間計算量

$P$ を期間数、$L=\max(\mathtt{minimum\_required\_levels})$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_cost_capacity_level_schedule` | $O(P(L+1))$ | $O(L+1)$ |

## 注意点

- 最低容量と3種類の単位費用は非負でなければならない。この前提により、最適水準を $L$ 以下に制限できる。
- 空の期間列には0を返す。
- 前提違反は `runtime_error`、結果が `long long` に収まらない場合は `overflow_error` を送出する。
