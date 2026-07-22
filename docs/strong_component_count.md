---
title: Strong Component Count (強連結成分数) [TJALG]
documentation_of: ../src/algorithm/graph/connectivity/strong_component_count.hpp
---

## strong_component_count

```cpp
int strong_component_count(const vector<vector<int>>& graph)
```

## 引数

- `graph`: 0-indexed頂点の有向隣接リスト。

## 戻り値

強連結成分の個数。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `strong_component_count` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- 各辺の行き先は $[0,N)$ でなければならない。違反時は `out_of_range` を送出する。
