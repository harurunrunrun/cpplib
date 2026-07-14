---
title: Perfect Assignments Count (完全割当て数) [ASSIGN]
documentation_of: ../src/algorithm/other/count_perfect_assignments.hpp
---

二値選好行列を満たす全単射割当ての個数を返す。

# count_perfect_assignments

```cpp
long long count_perfect_assignments(const vector<vector<int>>& preferences);
```

行を人、列を対象とし、値1の組だけを使う。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_perfect_assignments` | $O(N2^N)$ | $O(2^N)$ |

## 注意点

- 行列は正方かつ二値でなければならない。
- 答えが `long long` を超える場合は `overflow_error` を送出する。
