---
title: Hierarchy from Superior Constraints (上位関係制約からの階層構築) [MAKETREE]
documentation_of: ../../../../src/algorithm/graph/traversal/hierarchy_from_superior_constraints.hpp
---

「頂点 `u` は頂点 `v` の祖先でなければならない」という形の制約をすべて満たす根付き木を 1 つ構築する。

## `hierarchy_from_superior_constraints`

```cpp
vector<int> hierarchy_from_superior_constraints(
    const vector<vector<int>>& constraints
)
```

### 引数

- `constraints[u]`: 頂点 `u` を祖先に持たなければならない頂点の列。すなわち `v` が `constraints[u]` に含まれるとき、`u` は `v` の祖先でなければならない。

### 戻り値

全制約を満たす根付き木の親配列。根の親は `-1` である。実装は制約をグラフとみなしてトポロジカルソートし、その順に頂点を 1 列につないだパス（各頂点の親は直前の頂点）を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、制約の総数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `hierarchy_from_superior_constraints` | $O(N+M)$ | $O(N)$ |

## 注意点

- 制約に現れる頂点は $[0,N)$ でなければならない。
- 制約に閉路があれば `invalid_argument`、頂点が範囲外なら `runtime_error`、頂点数が `int` に収まらなければ `length_error` を送出する。
