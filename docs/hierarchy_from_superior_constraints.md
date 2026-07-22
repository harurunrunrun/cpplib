---
title: Hierarchy from Superior Constraints (上位関係制約からの階層構築) [MAKETREE]
documentation_of: ../src/algorithm/graph/traversal/hierarchy_from_superior_constraints.hpp
---

## hierarchy_from_superior_constraints

```cpp
vector<int> hierarchy_from_superior_constraints(
    const vector<vector<int>>& constraints
)
```

## 引数

- `constraints[u]`: 頂点 `u` が祖先でなければならない頂点の列。

## 戻り値

全制約を満たす根付き木の親配列。根の親は `-1`。

## API別の時間計算量・空間計算量

頂点数を $N$、制約数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `hierarchy_from_superior_constraints` | $O(N+M)$ | $O(N)$ |

## 注意点

- 制約端点は $[0,N)$ でなければならない。
- 制約にcycleがあれば `invalid_argument`、端点が範囲外なら `runtime_error` を送出する。
