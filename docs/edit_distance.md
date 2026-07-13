---
title: Edit Distance
documentation_of: ../src/algorithm/string/edit_distance.hpp
---

最小編集距離。

# 関数

```cpp
int edit_distance(a, b, insert_cost = 1, erase_cost = 1, replace_cost = 1)
```

挿入、削除、置換のコストを指定できる。

## 時間計算量

- $O(NM)$

## API別計算量

$N=|a|$, $M=|b|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `edit_distance(a,b,...)` | $O(NM)$ | $O(M)$ |

各挿入・削除・置換候補の評価は $O(1)$ とする。
