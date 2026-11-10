---
title: Minimum Cost Two-resource Cover (二資源被覆の最小費用) [SCUBADIV]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/resource/minimum_cost_two_resource_cover.hpp
---

2 種類の資源を供給する品物から部分集合を選び、両方の必要量を満たすための最小の費用を求める。必要量で打ち切った 2 次元の資源量を状態とする 0/1 ナップサック DP で計算する。

## `TwoResourceItem`

```cpp
struct TwoResourceItem {
    int first_resource;
    int second_resource;
    long long cost;
};
```

1 個の品物が供給する 2 種類の資源量と、その品物を選ぶ費用を表す。

## `minimum_cost_two_resource_cover`

```cpp
long long minimum_cost_two_resource_cover(
    int required_first,
    int required_second,
    const std::vector<TwoResourceItem>& items
);
```

各品物を高々 1 回選び、2 種類の資源量がそれぞれ必要量以上となるときの最小の費用を返す。達成できなければ $-1$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_cost_two_resource_cover` | $O(NR_1R_2)$ | $O(R_1R_2)$ |

## 注意点

- $R_1, R_2$ は 2 種類の必要量、$N$ は品物の個数である。
- 必要量、各品物の資源量、費用は非負でなければならない。
- 前提に違反した場合、状態数 $(R_1+1)(R_2+1)$ が `std::size_t` でオーバーフローする場合、または答えが `long long` に収まらない場合は `std::runtime_error` を送出する。
