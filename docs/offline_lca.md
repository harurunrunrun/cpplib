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

## 時間計算量

$Q$ をquery数とする。

- `offline_lca`: $O((N+Q)\alpha(N+1))$

木の検査・rooting、query隣接列の構築、Tarjan DFS、長さ $Q$ の回答出力を含む。

## 空間計算量

- 木のrooting情報、query列、DSU、戻り値を含めて $O(N+Q)$
