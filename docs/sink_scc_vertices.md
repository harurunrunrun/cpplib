---
title: Vertices in Sink Strongly Connected Components (シンク強連結成分に属する頂点) [BOTTOM]
documentation_of: ../src/algorithm/graph/sink_scc_vertices.hpp
---

有向グラフの強連結成分のうち、縮約graphで出次数が0となる成分に属する
すべての頂点を列挙する。

## `sink_scc_vertices`

```cpp
vector<int> sink_scc_vertices(const vector<vector<int>>& graph);
```

`graph[v]` を頂点 `v` から出る辺の行き先とする。
ほかの強連結成分へ出る辺を持たない強連結成分に属する頂点を、
頂点番号の昇順で返す。空graphには空vectorを返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sink_scc_vertices(graph)` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- 各辺の行き先は $[0,N)$ でなければならない。
- 自己loopと多重辺を許す。
- 辺端点が範囲外の場合は `out_of_range` を送出する。
