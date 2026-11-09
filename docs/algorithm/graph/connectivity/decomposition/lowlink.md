---
title: Lowlink (Lowlink法) [ABC075_C] [GRL_3_A] [GRL_3_B]
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/lowlink.hpp
---

無向グラフの関節点と橋を求める。

## 関数

```cpp
LowlinkResult lowlink(int n, const vector<pair<int, int>>& edges)
```

`edges` は無向辺の列。多重辺を扱える。

`LowlinkResult` は次のメンバを持つ。

```cpp
vector<int> ord;
vector<int> low;
vector<int> articulation;
vector<int> bridges;
```

- `ord[v]`: DFS で頂点 `v` を訪れた順番（行きがけ順）。
- `low[v]`: `v` から DFS 木の辺を下り、後退辺を高々 1 本使って到達できる頂点の `ord` の最小値。
- `articulation`: 関節点を昇順に並べたもの。
- `bridges`: 橋である入力辺の ID を昇順に並べたもの。

## 時間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$、橋の本数を $B$ とする。

- DFS と隣接リストの構築: $O(N + M)$
- 出力 ID のソート: $O(A \log (A + 1) + B \log (B + 1))$
- `lowlink` 全体: $O(N + M + A \log (A + 1) + B \log (B + 1))$

## 空間計算量

- 戻り値と隣接リストを含めて $O(N + M)$

## 注意点

- `n` は非負、辺の端点は $[0, n)$ でなければならない。違反時は `std::runtime_error` を送出する。記載した計算量には引数の検査と戻り値の構築を含む。
- DFS は再帰で実装されており、再帰の深さは最大 $N$ になる。
