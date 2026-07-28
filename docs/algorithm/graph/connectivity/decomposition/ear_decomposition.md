---
title: Ear Decomposition (耳分解)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/ear_decomposition.hpp
---

2頂点連結な無向多重グラフの全辺を、最初の閉路と、それまでに現れた頂点を両端に持つ開いた耳へ分解する。

## API

```cpp
optional<EarDecompositionResult> ear_decomposition(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

グラフが連結でない、関節点を持つ、または初期閉路を持たない場合は `nullopt` を返す。

```cpp
struct EarDecompositionResult {
    vector<vector<int>> ears;
    vector<vector<int>> ear_edge_ids;
};
```

- `ears[0]`: 最初の閉路を巡回順に並べた頂点列。先頭頂点を末尾に重ねない
- `ear_edge_ids[0]`: 最初の閉路を同じ巡回順に並べた入力辺ID
- `ears[i]` (`i > 0`): 開いた耳の始点、内部頂点、終点
- `ear_edge_ids[i]`: その耳を順に結ぶ入力辺ID

各入力辺はちょうど1個の `ear_edge_ids` に現れる。最初の耳以外では両端だけが以前の耳に現れ、内部頂点は初出である。すでに現れた2頂点を結ぶ未使用辺は、辺1本の耳になる。

## 時間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$、橋数を $B$ とする。

- 連結性・関節点・初期閉路の検査: $O(N+M+A\log(A+1)+B\log(B+1))$
- 未追加頂点成分からの耳抽出: 最悪 $O(N(N+M))$
- 残りの辺1本耳の追加: $O(M)$

## 空間計算量

$O(N+M)$。

## 注意点

辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。多重辺2本からなる初期閉路を扱える。自己ループを含む入力と、2頂点未満のグラフには `nullopt` を返す。DFSによる初期閉路探索とLowlink計算は再帰を使う。
