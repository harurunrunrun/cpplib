---
title: Ear Decomposition (耳分解)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/ear_decomposition.hpp
---

二重頂点連結な無向多重グラフの全辺を、最初の閉路と、それまでに現れた頂点を両端に持つ開いた耳（パス）とに分解する。

## API

```cpp
optional<EarDecompositionResult> ear_decomposition(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

グラフが連結でない、関節点を持つ、または初期閉路を持たない場合は `std::nullopt` を返す。

```cpp
struct EarDecompositionResult {
    vector<vector<int>> ears;
    vector<vector<int>> ear_edge_ids;
};
```

- `ears[0]`: 最初の閉路を巡回順に並べた頂点列。先頭の頂点を末尾に重ねて書かない
- `ear_edge_ids[0]`: 最初の閉路の入力辺の ID を、同じ巡回順に並べたもの
- `ears[i]`（$i \gt 0$）: 開いた耳の始点、内部頂点、終点の順に並べた頂点列
- `ear_edge_ids[i]`: その耳を順に結ぶ入力辺の ID

各入力辺は、ちょうど 1 個の `ear_edge_ids` に現れる。最初の耳以外では両端だけがそれ以前の耳に現れ、内部頂点は初出である。すでに現れた 2 頂点を結ぶ未使用の辺は、辺 1 本だけの耳になる。

## 時間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$、橋の本数を $B$ とする。

- 連結性・関節点・初期閉路の検査: $O(N + M + A \log (A + 1) + B \log (B + 1))$
- 未追加の頂点からなる成分からの耳の抽出: 最悪 $O(N(N + M))$
- 残った辺 1 本の耳の追加: $O(M)$

## 空間計算量

$O(N + M)$。

## 注意点

- `vertex_count` は非負、辺の端点は $[0, N)$ でなければならない。違反時は `std::runtime_error` を送出する。
- 多重辺 2 本からなる初期閉路を扱える。
- 自己ループを含む入力、頂点数 2 未満のグラフ、辺のないグラフには `std::nullopt` を返す。
- 初期閉路を探す DFS（`std::function` による）と Lowlink 計算は再帰を使うため、再帰の深さは最大 $N$ になる。
