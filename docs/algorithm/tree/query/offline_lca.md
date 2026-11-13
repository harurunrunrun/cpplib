---
title: Offline LCA (オフライン最小共通祖先)
documentation_of: ../../../../src/algorithm/tree/query/offline_lca.hpp
---

Tarjan のオフライン LCA アルゴリズムを用いる。
クエリ列をまとめて与え、各クエリの LCA を入力順に返す。

## API

```cpp
std::vector<int> offline_lca(
    const std::vector<std::vector<int>>& graph,
    const std::vector<std::pair<int, int>>& queries,
    int root = 0
)
```

- `graph` は無向木の隣接リストである。
- `queries[i] = {u, v}` に対する LCA を、戻り値の `i` 番目に格納する。

## 時間計算量

$Q$ をクエリ数とする。

- `offline_lca`: $O((N+Q)\alpha(N+1))$

木の検査と根付け、クエリの隣接列の構築、Tarjan の DFS、長さ $Q$ の回答の出力を含む。

## 空間計算量

木の根付けの情報、クエリ列、DSU、戻り値を含めて $O(N+Q)$ である。

## 注意点

- 頂点と `root` は範囲内でなければならない。空木を除き、入力は無向の連結木とする。
- 空木では `queries` も空でなければならない。
- 端点・クエリの頂点・`root` の範囲外、閉路、辺数、連結性の違反時は `std::runtime_error` を送出する。記載した計算量には木の検査を含む。
