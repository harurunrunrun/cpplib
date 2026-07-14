---
title: Minimum String Break Cost (文字列切断の最小費用) [BRKSTRNG]
documentation_of: ../src/algorithm/other/minimum_string_break_cost.hpp
---

指定位置ですべて切断するときの最小総費用を返す。

# minimum_string_break_cost

```cpp
long long minimum_string_break_cost(
    long long length, const vector<long long>& break_positions
);
```

長さ `length` の区間をすべての `break_positions` で切る。1回の費用はその時点で切る断片の長さである。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_string_break_cost` | $O(M^2)$ | $O(M^2)$ |

## 注意点

- 切断位置は狭義昇順で $(0,length)$ 内になければならない。
- 前提違反時は `runtime_error` を送出する。
