---
title: Offline LCA (オフライン最小共通祖先)
documentation_of: ../src/algorithm/tree/query/offline_lca.hpp
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

## 注意点

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
