---
title: Unique Sink SCC Vertices (唯一のシンク強連結成分頂点) [CAPCITY]
documentation_of: ../src/algorithm/graph/connectivity/unique_sink_scc_vertices.hpp
---

有向グラフの強連結成分を縮約したDAGで、出次数0の成分がただ1つだけ存在するとき、その成分の頂点を返す。

## unique_sink_scc_vertices

```cpp
vector<int> unique_sink_scc_vertices(
    const vector<vector<int>>& graph
);
```

## 引数

- `graph[v]`: 頂点 `v` から出る辺の行き先。

## 戻り値

シンク強連結成分が一意なら、その頂点番号を昇順で返す。空グラフ、またはシンク強連結成分が複数ある場合は空vectorを返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、戻り値の要素数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `unique_sink_scc_vertices` | $O(N+M+K\log(K+1))$ | $O(N+M)$ |

## 注意点

- 各行き先は $[0,N)$ でなければならない。違反時は `out_of_range` を送出する。
- 自己ループと多重辺を許す。
