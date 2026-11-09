---
title: Two Edge Connected Components (二重辺連結成分分解) [two_edge_connected_components]
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/two_edge_connected_components.hpp
---

無向グラフの二重辺連結成分分解。橋を取り除いた後の連結成分を求める。

## 関数

```cpp
TwoEdgeConnectedComponentsResult two_edge_connected_components(
    int n,
    const vector<pair<int, int>>& edges
)
```

`TwoEdgeConnectedComponentsResult` は次のメンバを持つ。

```cpp
int count;
vector<int> id;
vector<vector<int>> groups;
vector<int> bridges;
```

- `count`: 成分の個数。
- `id[v]`: 頂点 `v` が属する成分の番号。
- `groups[c]`: 成分 `c` の頂点列。
- `bridges`: 橋である入力辺の ID を昇順に並べたもの。

## 時間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$、橋の本数を $B$ とする。

- Lowlink と成分を求める DFS: $O(N + M)$
- 関節点と橋の ID のソート（`lowlink` 内）: $O(A \log (A + 1) + B \log (B + 1))$
- `two_edge_connected_components` 全体: $O(N + M + A \log (A + 1) + B \log (B + 1))$

全頂点を `groups` へ出力する時間を含む。

## 空間計算量

- 戻り値と隣接リストを含めて $O(N + M)$

## 注意点

- `n` は非負、辺の端点は $[0, n)$ でなければならない。違反時は `std::runtime_error` を送出する。記載した計算量には引数の検査と戻り値の構築を含む。
- 多重辺を扱える。
- 内部で呼ぶ `lowlink` の DFS は再帰で実装されており、再帰の深さは最大 $N$ になる。
