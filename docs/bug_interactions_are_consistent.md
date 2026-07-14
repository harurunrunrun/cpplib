---
title: Bug Interactions Are Consistent (虫の相互作用整合性) [BUGLIFE]
documentation_of: ../src/algorithm/graph/bug_interactions_are_consistent.hpp
---

異なる二群間でのみ相互作用できるという条件に、観測された相互作用が矛盾しないかを判定する。

# bug_interactions_are_consistent

```cpp
bool bug_interactions_are_consistent(
    int bug_count,
    const vector<pair<int, int>>& interactions
);
```

各相互作用を無向辺とみなし、グラフが二部グラフなら `true`、そうでなければ `false` を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、相互作用数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `bug_interactions_are_consistent` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- 頂点番号は `0` 以上 `bug_count` 未満でなければならない。
- `bug_count` が負、または頂点番号が範囲外の場合は `runtime_error` を送出する。
- 自己ループが含まれる場合は `false` を返す。
