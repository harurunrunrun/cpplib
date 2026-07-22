---
title: Minimum Exact Weight Cost with Item Limit (個数制限付き指定重量の最小費用) [ABA12C]
documentation_of: ../src/algorithm/other/dynamic_programming/minimum_exact_weight_cost_with_item_limit.hpp
---

正整数の重みを持つ品物を個数制限内で重複選択し、指定した合計重みを作る
最小費用を求める。

## `minimum_exact_weight_cost_with_item_limit`

```cpp
long long minimum_exact_weight_cost_with_item_limit(
    int target_weight,
    int maximum_item_count,
    const vector<long long>& cost_by_weight
);
```

`cost_by_weight[w-1]` は重み $w$ の品物1個の費用で、$-1$ はその重みを
使用できないことを表す。同じ重みは何度でも選べる。合計重み
`target_weight` を高々 `maximum_item_count` 個で作れない場合は $-1$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_exact_weight_cost_with_item_limit` | $O(CT\min(T,W))$ | $O(T)$ |

## 注意点

- $T$ は `target_weight`、$C=\min(\mathrm{maximum\_item\_count},T)$、
  $W$ は費用表の長さである。
- 目標・個数制限は非負、各費用は $-1$ または非負でなければならない。
- 前提違反または答えが `long long` に収まらない場合は `runtime_error` を送出する。
