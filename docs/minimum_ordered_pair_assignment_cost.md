---
title: Minimum Ordered-Pair Assignment Cost (順序制約付きペア割当ての最小費用) [MPILOT]
documentation_of: ../src/algorithm/other/minimum_ordered_pair_assignment_cost.hpp
---

順序付き列の各要素をopening役またはclosing役へ割り当て、openingが必ず対応するclosingより前に現れるペアを作るときの最小総費用を求める。

## OrderedPairAssignmentCosts

```cpp
struct OrderedPairAssignmentCosts {
    long long opening_cost;
    long long closing_cost;
};
```

1要素をそれぞれの役へ割り当てる費用を表す。費用は負でもよい。

## minimum_ordered_pair_assignment_cost

```cpp
long long minimum_ordered_pair_assignment_cost(
    const vector<OrderedPairAssignmentCosts>& ordered_costs
);
```

各要素をちょうど1つの役へ割り当て、openingとclosingの個数を等しくする。任意の接頭辞でopening数がclosing数以上になる割当て、すなわち各openingをそれより後のclosingと一対一対応できる割当ての最小総費用を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_ordered_pair_assignment_cost` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 要素数は偶数でなければならず、違反時は `runtime_error` を送出する。
- 空の入力には0を返す。
- 要素数を内部添字で表せない場合は `length_error`、結果が `long long` に収まらない場合は `overflow_error` を送出する。
