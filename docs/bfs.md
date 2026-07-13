---
title: Breadth First Search (幅優先探索)
documentation_of: ../src/algorithm/graph/bfs.hpp
---

重みなし有向グラフの最短路を幅優先探索で求める。無向グラフは各辺を両方向に追加する。

# 関数

```cpp
BFSResult bfs(graph, source)
BFSResult bfs(graph, sources)
```

`graph[v]` は行き先頂点番号の列。`sources` は始点の列で、重複してもよい。空なら全頂点が到達不能になる。

返り値は次を持つ。

```cpp
vector<int> dist;
vector<int> source;
vector<int> parent;
vector<char> reachable;
vector<int> path(int target) const;
```

`dist[v]` は最短距離で、到達不能なら `-1`。`source[v]` は採用された始点、`parent[v]` は最短路木の親であり、始点と到達不能頂点では `-1` になる。同距離の候補が複数ある場合は、先に探索されたものを採用する。

`path(target)` は対応する始点から `target` までの頂点列を返す。到達不能なら空列を返す。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を辺数、$S$ を入力した始点数、$P$ を復元するpathの頂点数とする。

- `bfs(graph, source)`: $O(N+M)$
- `bfs(graph, sources)`: $O(N+M+S)$
- `BFSResult::path`: $O(P)$ 時間、返り値に $O(P)$ 空間
- 探索中の追加空間と返り値: $O(N)$

## 注意点

`BFSResult::path(target)` は両端を含むpathを返す。到達不能なら空列、targetが範囲外なら `runtime_error`。単一・複数始点版の全始点も範囲検査する。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
