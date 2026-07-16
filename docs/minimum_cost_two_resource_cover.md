---
title: Minimum Cost Two-resource Cover (二資源被覆の最小費用) [SCUBADIV]
documentation_of: ../src/algorithm/other/minimum_cost_two_resource_cover.hpp
---

2種類の資源量を持つ品物から部分集合を選び、両方の必要量を満たす最小費用を求める。

## `TwoResourceItem`

```cpp
struct TwoResourceItem {
    int first_resource;
    int second_resource;
    long long cost;
};
```

1個の品物が供給する2種類の資源量と、その品物を選ぶ費用を表す。

## `minimum_cost_two_resource_cover`

```cpp
long long minimum_cost_two_resource_cover(
    int required_first,
    int required_second,
    const vector<TwoResourceItem>& items
);
```

各品物を高々1回選び、2種類の資源量がそれぞれ必要量以上となる最小費用を返す。
達成不能なら $-1$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_cost_two_resource_cover` | $O(NR_1R_2)$ | $O(R_1R_2)$ |

## 注意点

- $R_1,R_2$ は2種類の必要量、$N$ は品物数である。
- 必要量、各品物の資源量、費用は非負でなければならない。
- 前提違反、状態数の積のオーバーフロー、または答えが `long long` に
  収まらない場合は `runtime_error` を送出する。
