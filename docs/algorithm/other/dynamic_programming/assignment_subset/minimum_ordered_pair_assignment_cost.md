---
title: Minimum Ordered-Pair Assignment Cost (順序制約付きペア割当ての最小費用) [MPILOT]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/minimum_ordered_pair_assignment_cost.hpp
---

順序の付いた列の各要素を opening 役または closing 役に割り当て、opening が必ず対応する closing より前に現れるペアを作るときの、最小の総費用を求める。

## OrderedPairAssignmentCosts

```cpp
struct OrderedPairAssignmentCosts {
    long long opening_cost;
    long long closing_cost;
};
```

1 つの要素をそれぞれの役に割り当てる費用を表す。費用は負でもよい。

## minimum_ordered_pair_assignment_cost

```cpp
long long minimum_ordered_pair_assignment_cost(
    const std::vector<OrderedPairAssignmentCosts>& ordered_costs
);
```

各要素をちょうど 1 つの役に割り当て、opening と closing の個数を等しくする。任意の接頭辞で opening の個数が closing の個数以上になる割当て、すなわち各 opening をそれより後ろの closing と一対一に対応させられる割当てのうち、最小の総費用を返す。

全要素をいったん closing とし、先頭から見て接頭辞の条件が破れるたびに、それまでの要素のうち opening への変更費用（`opening_cost - closing_cost`）が最小のものを優先度付きキューから取り出して opening に変える貪欲法で求める。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_ordered_pair_assignment_cost` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 要素数は偶数でなければならず、違反時は `std::runtime_error` を送出する。
- 空の入力には $0$ を返す。
- 要素数が `int` の範囲を超える場合は `std::length_error`、結果が `long long` に収まらない場合は `std::overflow_error` を送出する。中間値は `__int128` で計算する。
