---
title: 0-1 BFS (0-1幅優先探索)
documentation_of: ../../../../../src/algorithm/graph/shortest_path/source/zero_one_bfs.hpp
---

辺の重みが $0$ または $1$ の有向グラフの最短路を両端キュー（`std::deque`）で求める。無向グラフでは各辺を両方向に追加する。

## 関数

```cpp
struct ZeroOneBFSEdge { int to; int cost; };

ZeroOneBFSResult zero_one_bfs(const std::vector<std::vector<ZeroOneBFSEdge>>& graph, int source)
ZeroOneBFSResult zero_one_bfs(const std::vector<std::vector<ZeroOneBFSEdge>>& graph, const std::vector<int>& sources)
```

`sources` は始点の列で、重複してもよい。`cost` が $0$, $1$ 以外なら例外を送出する。

`ZeroOneBFSResult` は `BFSResult` の別名であり、メンバ `dist`, `source`, `parent`, `reachable` と `path(target)` をもつ。

- `dist[v]`: 最短距離。到達不能なら `-1`。
- `source[v]`: 最短路の始点。到達不能なら `-1`。
- `parent[v]`: 最短路における直前の頂点。始点と到達不能な頂点では `-1`。
- `path(target)`: 始点から `target` への最短路を頂点列で返す。到達不能なら空列を返す。

```cpp
template <class EnumerateEdges>
ZeroOneBFSResult zero_one_bfs_implicit(int vertex_count, int source, EnumerateEdges enumerate_edges)
template <class EnumerateEdges>
ZeroOneBFSResult zero_one_bfs_implicit(int vertex_count, const std::vector<int>& sources, EnumerateEdges enumerate_edges)
```

隣接リストを構築せず、頂点を両端キューから初めて取り出したときだけ辺を列挙する。`enumerate_edges(v, relax)` は、`v` から出る各辺について、その呼び出しの間に `relax(to, cost)` を 1 回ずつ呼ぶ関数とする。巨大な格子グラフなど、頂点から隣接辺をその場で求められる場合に用いる。

## 計算量

$N$ を頂点数、$M$ を辺数、$S$ を入力した始点の個数、$P$ を復元するパスの頂点数とする。時間計算量は次のとおりである。

- `zero_one_bfs(graph, source)`: $O(N + M)$
- `zero_one_bfs(graph, sources)`: $O(N + M + S)$
- `zero_one_bfs_implicit(vertex_count, source, enumerate_edges)`: $O(N + M)$
- `zero_one_bfs_implicit(vertex_count, sources, enumerate_edges)`: $O(N + M + S)$
- `ZeroOneBFSResult::path`: $O(P)$ 時間、戻り値に $O(P)$ 空間
- 空間計算量: 隣接リスト版は探索と戻り値で $O(N + M)$、implicit 版は $O(N + M)$（両端キューには辺の本数分まで積まれうる。`enumerate_edges` が使う領域を除く）

## 注意点

- 始点と辺の行き先は $[0, N)$ の範囲内、辺の重みは $0$ または $1$ でなければならない。違反時は `runtime_error` を送出する。`path` の引数が範囲外の場合も `runtime_error` を送出する。
- implicit 版は、到達した頂点から列挙された辺だけを検査する。`enumerate_edges` は渡された `relax` を呼び出しの後まで保持してはならない。
