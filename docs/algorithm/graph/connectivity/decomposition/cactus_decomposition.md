---
title: Cactus Decomposition (カクタス分解)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/cactus_decomposition.hpp
---

無向グラフがカクタスであるかを判定し、辺を橋と単純閉路へ分解する。カクタスとは、各辺が高々 1 個の単純閉路に属するグラフである。

## API

```cpp
optional<CactusDecompositionResult> cactus_decomposition(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

カクタスでなければ `std::nullopt` を返す。

```cpp
struct CactusDecompositionResult {
    vector<int> bridge_edge_ids;
    vector<vector<int>> cycles;
    vector<vector<int>> cycle_edge_ids;
};
```

- `bridge_edge_ids`: どの閉路にも属さない入力辺の ID（昇順）
- `cycles[i]`: `i` 番目の単純閉路を巡回順に並べた頂点列
- `cycle_edge_ids[i]`: `cycles[i]` と同じ巡回順に並べた入力辺の ID

すべての入力辺は、`bridge_edge_ids` といずれか 1 個の `cycle_edge_ids` のどちらかにちょうど 1 回現れる。

## 時間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$、2 本以上の辺を持つ二重頂点連結成分の個数を $C$ とする。

- 二重頂点連結成分分解: $O(N + M \log (M + 1) + A \log (A + 1))$
- 各成分のカクタス判定と閉路の復元: $O(NC + M)$（成分ごとに長さ $N$ の隣接リストを確保するため）
- 全体: $O(NC + M \log (M + 1) + A \log (A + 1))$

## 空間計算量

$O(N + M)$。

## 注意点

- `vertex_count` は非負、辺の端点は $[0, N)$ でなければならない。違反時は `std::runtime_error` を送出する。
- 多重辺 2 本からなる長さ 2 の閉路を扱える。
- 自己ループを含む入力はカクタスとして扱わず、`std::nullopt` を返す。
- 非連結グラフでも、各連結成分がカクタスなら分解できる。
- 内部で使う二重頂点連結成分分解の DFS は再帰で実装されており、再帰の深さは最大 $N$ になる。
