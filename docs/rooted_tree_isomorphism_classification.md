---
title: Rooted Tree Isomorphism Classification (根付き部分木同型分類) [rooted_tree_isomorphism_classification]
documentation_of: ../src/algorithm/tree/rooted_tree_isomorphism_classification.hpp
---

根付き木の全頂点について、その頂点を根とする部分木を同型類へ分類する。

## RootedTreeIsomorphismClassification

```cpp
struct RootedTreeIsomorphismClassification {
    int class_count;
    vector<int> class_id;
};
```

- `class_count`: 相異なる同型類の個数。
- `class_id[v]`: 頂点 `v` の部分木の同型類番号。範囲は
  `[0, class_count)` で、同型な部分木に限り同じ値となる。

## rooted_tree_isomorphism_classification

```cpp
RootedTreeIsomorphismClassification
rooted_tree_isomorphism_classification(
    const vector<vector<int>>& graph,
    int root = 0
)
```

無向木 `graph` を `root` で根付け、全頂点の分類結果を返す。
空木では `root == 0` に限り空の結果を返す。

## 時間計算量

頂点数を $N$ として $O(N)$。部分木の高さ順に同型類を確定し、子の
同型類ごとの出現回数でpartitionを疎にrefineする。各親子辺は一度だけ
個数集計へ寄与するため、比較sort、連想配列、衝突し得るhashには依存しない。

## 空間計算量

根付き木情報、partition、戻り値を含めて $O(N)$。

## 注意点

- 頂点番号と `root` は0-indexed。
- 非空入力は無向の連結木でなければならない。端点範囲、辺数、閉路、
  連結性、`root` に違反した場合は `runtime_error` を送出する。
- `class_id` の具体的な番号は同型関係だけを表し、永続的な識別子ではない。
