---
title: Perfect Assignments Count (完全割当て数) [ASSIGN]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/count_perfect_assignments.hpp
---

0/1 の選好行列を満たす全単射の割当ての個数を返す。

## count_perfect_assignments

```cpp
long long count_perfect_assignments(const std::vector<std::vector<int>>& preferences);
```

行を人、列を対象とし、値が $1$ の組だけを使う。使用済みの対象の集合を状態とするビット DP で数える。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_perfect_assignments` | $O(N2^N)$ | $O(2^N)$ |

## 注意点

- $N \le 30$ でなければならない。行列は正方かつ各要素が $0$ または $1$ でなければならない。違反時は `std::runtime_error` を送出する。
- DP の途中の値が `long long` を超える場合は `std::overflow_error` を送出する。
