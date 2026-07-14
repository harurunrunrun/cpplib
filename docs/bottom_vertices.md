---
title: Bottom Vertices (有向グラフの底頂点) [BOTTOM]
documentation_of: ../src/algorithm/graph/bottom_vertices.hpp
---

# bottom_vertices

```cpp
vector<int> bottom_vertices(const vector<vector<int>>& graph)
```

## 引数

- `graph`: 0-indexed頂点の有向隣接リスト。

## 戻り値

頂点から到達可能な任意の頂点から元の頂点へ戻れる、すべての頂点を昇順で返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `bottom_vertices` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- 各辺の行き先は $[0,N)$ でなければならない。違反時は `out_of_range` を送出する。
