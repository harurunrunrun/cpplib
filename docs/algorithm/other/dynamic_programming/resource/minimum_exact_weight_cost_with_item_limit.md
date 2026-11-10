---
title: Minimum Exact Weight Cost with Item Limit (個数制限付き指定重量の最小費用) [ABA12C]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/resource/minimum_exact_weight_cost_with_item_limit.hpp
---

正の整数の重みをもつ品物を、個数の上限の範囲で重複を許して選び、指定した合計重みをちょうど作るための最小の費用を求める。使った個数ごとに合計重みを状態とする DP で計算する。

## `minimum_exact_weight_cost_with_item_limit`

```cpp
long long minimum_exact_weight_cost_with_item_limit(
    int target_weight,
    int maximum_item_count,
    const std::vector<long long>& cost_by_weight
);
```

`cost_by_weight[w-1]` は重み $w$ の品物 1 個の費用で、$-1$ はその重みの品物を使えないことを表す。同じ重みの品物は何度でも選べる。合計重み `target_weight` を高々 `maximum_item_count` 個の品物で作れない場合は $-1$ を返す。`target_weight == 0` なら $0$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_exact_weight_cost_with_item_limit` | $O(CT\min(T,W))$ | $O(T)$ |

## 注意点

- $T$ は `target_weight`、$K$ は `maximum_item_count`、$C=\min(K,T)$、$W$ は費用表の長さである。
- 目標の重みと個数の上限は非負、各費用は $-1$ または非負でなければならない。
- 前提に違反した場合、費用表の長さが `int` の範囲を超える場合、または答えが `long long` に収まらない場合は `std::runtime_error` を送出する。
