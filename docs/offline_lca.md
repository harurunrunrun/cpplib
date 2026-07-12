---
title: Offline LCA
documentation_of: ../src/algorithm/tree/offline_lca.hpp
---

Tarjan の offline LCA。

クエリ列をまとめて与え、各クエリの LCA を入力順に返す。

## 関数

- `offline_lca(graph, queries, root = 0)`
    - `graph` は無向木の隣接リスト。
    - `queries[i] = {u, v}` に対する LCA を返す。

## 計算量

- $O((n + q)\alpha(n))$
