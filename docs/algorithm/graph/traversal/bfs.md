---
title: Breadth First Search (幅優先探索)
documentation_of: ../../../../src/algorithm/graph/traversal/bfs.hpp
---

重みなし有向グラフの最短路を幅優先探索で求める。無向グラフは各辺を両方向に追加して表す。

## 関数

```cpp
BFSResult bfs(const vector<vector<int>>& graph, int source)
BFSResult bfs(const vector<vector<int>>& graph, const vector<int>& sources)
```

`graph[v]` は頂点 `v` から出る辺の行き先頂点番号の列である。`sources` は始点の列で、重複してもよい。空なら全頂点が到達不能になる。

戻り値は次のメンバを持つ。

```cpp
vector<int> dist;
vector<int> source;
vector<int> parent;
vector<char> reachable;
vector<int> path(int target) const;
```

`dist[v]` は最短距離で、到達不能なら `-1` である。`source[v]` は採用された始点、`parent[v]` は最短路木での親であり、到達不能な頂点ではどちらも `-1`、始点では `parent[v] == -1` になる。`reachable[v]` は到達可能なら `1` である。同じ距離の候補が複数ある場合は、先に探索されたもの（`sources` の中では先に現れた始点）を採用する。

`path(target)` は対応する始点から `target` までの頂点列を、両端を含めて返す。到達不能なら空列を返す。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を辺数、$S$ を入力した始点の個数、$P$ を復元するパスの頂点数とする。

- `bfs(graph, source)`: $O(N+M)$
- `bfs(graph, sources)`: $O(N+M+S)$
- `BFSResult::path`: $O(P)$ 時間、戻り値に $O(P)$ 空間
- 探索中の追加空間と戻り値: $O(N)$

記載した計算量には引数の検査と戻り値の構築を含む。

## 注意点

- 隣接リストの行き先と始点は $[0,N)$ に入っていなければならない。違反時は `runtime_error` を送出する。単一始点版・複数始点版ともに全始点を範囲検査する。
- `path(target)` に範囲外の `target` を渡すと `runtime_error` を送出する。
