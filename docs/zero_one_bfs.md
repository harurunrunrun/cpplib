---
title: 0-1 BFS (0-1幅優先探索)
documentation_of: ../src/algorithm/graph/zero_one_bfs.hpp
---

辺重みが $0$ または $1$ の有向グラフの最短路をdequeで求める。無向グラフは各辺を両方向に追加する。

## 関数

```cpp
struct ZeroOneBFSEdge { int to; int cost; };

ZeroOneBFSResult zero_one_bfs(graph, source)
ZeroOneBFSResult zero_one_bfs(graph, sources)
```

`sources` は始点の列で、重複してもよい。`cost` が $0,1$ 以外なら例外を送出する。

`ZeroOneBFSResult` は `BFSResult` と同じ `dist`, `source`, `parent`, `reachable` および `path(target)` を持つ。`dist[v]` は到達不能なら `-1`。

```cpp
ZeroOneBFSResult zero_one_bfs_implicit(vertex_count, source, enumerate_edges)
ZeroOneBFSResult zero_one_bfs_implicit(vertex_count, sources, enumerate_edges)
```

隣接リストを構築せず、頂点をdequeから初めて取り出したときだけ辺を列挙する。
`enumerate_edges(v, relax)` は `v` から出る各辺について、その呼び出し中に `relax(to, cost)` を1回ずつ呼ぶ関数とする。
巨大な格子グラフなど、頂点から隣接辺をその場で求められる場合に用いる。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を辺数、$S$ を入力した始点数、$P$ を復元するpathの頂点数とする。

- `zero_one_bfs(graph, source)`: $O(N+M)$
- `zero_one_bfs(graph, sources)`: $O(N+M+S)$
- `zero_one_bfs_implicit(vertex_count, source, enumerate_edges)`: $O(N+M)$
- `zero_one_bfs_implicit(vertex_count, sources, enumerate_edges)`: $O(N+M+S)$
- `ZeroOneBFSResult::path`: $O(P)$ 時間、返り値に $O(P)$ 空間
- 隣接リスト版の探索と返り値: $O(N+M)$
- implicit graph版の探索と返り値: $O(N)$（`enumerate_edges` が使用する領域を除く）

## 注意点

頂点引数と列挙した辺の行き先は、各APIで定めた頂点範囲内でなければならず、辺重みは $0$ または $1$ とする。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
implicit graph版は到達した頂点から列挙された辺だけを検査する。`enumerate_edges` は渡された `relax` を呼び出し後に保持してはならない。
