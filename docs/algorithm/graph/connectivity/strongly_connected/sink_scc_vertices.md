---
title: Vertices in Sink Strongly Connected Components (シンク強連結成分に属する頂点) [BOTTOM]
documentation_of: ../../../../../src/algorithm/graph/connectivity/strongly_connected/sink_scc_vertices.hpp
---

有向グラフの強連結成分のうち、縮約したグラフで出次数が 0 になる成分に属するすべての頂点を列挙する。

## `sink_scc_vertices`

```cpp
vector<int> sink_scc_vertices(const vector<vector<int>>& graph);
```

`graph[v]` は頂点 `v` から出る辺の行き先の列である。
ほかの強連結成分へ出る辺を持たない強連結成分に属する頂点を、頂点番号の昇順で返す。頂点数 0 のグラフには空の列を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sink_scc_vertices(graph)` | $O(N + M)$ | $O(N + M)$ |

## 注意点

- 各辺の行き先は $[0, N)$ でなければならない。範囲外の場合は `std::out_of_range` を送出する。
- 自己ループと多重辺を許す。
