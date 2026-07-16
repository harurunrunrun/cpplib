---
title: Exactly Once Guard Coverage (警備範囲一意被覆判定) [AKBAR]
documentation_of: ../src/algorithm/graph/exactly_once_guard_coverage.hpp
---

## has_exactly_once_guard_coverage

```cpp
bool has_exactly_once_guard_coverage(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    const vector<pair<int, int>>& guards
)
```

## 引数

- `vertex_count`: 頂点数。
- `edges`: 0-indexed端点を持つ無向辺列。
- `guards[i] = {base, strength}`: 警備拠点と非負の警備距離。

## 戻り値

全頂点がちょうど1人の警備範囲に含まれるなら真。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `has_exactly_once_guard_coverage` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- `vertex_count` と `strength` は非負、辺端点と `base` は範囲内でなければならない。
- 前提違反時は `invalid_argument` または `out_of_range` を送出する。
