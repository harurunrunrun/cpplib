---
title: 0-1 BFS (0-1幅優先探索)
documentation_of: ../src/algorithm/graph/zero_one_bfs.hpp
---

辺重みが $0$ または $1$ の有向グラフの最短路をdequeで求める。無向グラフは各辺を両方向に追加する。

# 関数

```cpp
struct ZeroOneBFSEdge { int to; int cost; };

ZeroOneBFSResult zero_one_bfs(graph, source)
ZeroOneBFSResult zero_one_bfs(graph, sources)
```

`sources` は始点の列で、重複してもよい。`cost` が $0,1$ 以外なら例外を送出する。

`ZeroOneBFSResult` は `BFSResult` と同じ `dist`, `source`, `parent`, `reachable` および `path(target)` を持つ。`dist[v]` は到達不能なら `-1`。

# 計算量

$N$ を頂点数、$M$ を辺数、$S$ を入力した始点数、$P$ を復元するpathの頂点数とする。

- `zero_one_bfs(graph, source)`: $O(N+M)$
- `zero_one_bfs(graph, sources)`: $O(N+M+S)$
- `ZeroOneBFSResult::path`: $O(P)$ 時間、返り値に $O(P)$ 空間
- 探索中のdeque・状態配列と返り値: $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
