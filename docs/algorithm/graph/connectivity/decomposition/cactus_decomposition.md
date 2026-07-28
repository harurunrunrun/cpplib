---
title: Cactus Decomposition (カクタス分解)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/cactus_decomposition.hpp
---

無向グラフがカクタスであるかを判定し、辺を橋と単純閉路へ分解する。カクタスとは各辺が高々1個の単純閉路に属するグラフである。

## API

```cpp
optional<CactusDecompositionResult> cactus_decomposition(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

カクタスでなければ `nullopt` を返す。

```cpp
struct CactusDecompositionResult {
    vector<int> bridge_edge_ids;
    vector<vector<int>> cycles;
    vector<vector<int>> cycle_edge_ids;
};
```

- `bridge_edge_ids`: どの閉路にも属さない入力辺のID。昇順
- `cycles[i]`: `i` 番目の単純閉路を巡回順に並べた頂点列
- `cycle_edge_ids[i]`: `cycles[i]` と同じ巡回順の入力辺ID

すべての入力辺は `bridge_edge_ids` またはいずれか1個の `cycle_edge_ids` にちょうど1回現れる。

## 時間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$、橋数を $B$ とする。

- 二重頂点連結成分分解: $O(N+M+A\log(A+1)+B\log(B+1))$
- 各成分のカクタス判定と閉路復元: $O(N+M)$
- 全体: $O(N+M+A\log(A+1)+B\log(B+1))$

## 空間計算量

$O(N+M)$。

## 注意点

辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。多重辺2本からなる長さ2の閉路を扱える。自己ループを含む入力は、このAPIではカクタスとして扱わず `nullopt` を返す。非連結グラフも各連結成分がカクタスなら分解できる。
