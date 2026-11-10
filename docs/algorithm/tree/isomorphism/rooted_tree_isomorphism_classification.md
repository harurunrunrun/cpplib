---
title: Rooted Tree Isomorphism Classification (根付き部分木同型分類) [rooted_tree_isomorphism_classification]
documentation_of: ../../../../src/algorithm/tree/isomorphism/rooted_tree_isomorphism_classification.hpp
---

根付き木の全頂点について、その頂点を根とする部分木を同型類に分類する。

## RootedTreeIsomorphismClassification

```cpp
struct RootedTreeIsomorphismClassification{
    int class_count = 0;
    std::vector<int> class_id;
};
```

- `class_count`: 相異なる同型類の個数。
- `class_id[v]`: 頂点 `v` の部分木の同型類の番号。範囲は `[0, class_count)` で、同型な部分木に限り同じ値となる。

## rooted_tree_isomorphism_classification

```cpp
RootedTreeIsomorphismClassification
rooted_tree_isomorphism_classification(
    const std::vector<std::vector<int>>& graph,
    int root = 0
)
```

無向木 `graph` を `root` で根付き木にし、全頂点の分類結果を返す。
空木では、`root == 0` の場合に限り空の結果を返す（それ以外は `std::runtime_error`）。
内部では `rooted_tree_isomorphism_labels` を使う。

## 時間計算量

頂点数を $N$ として $O(N)$ である。
部分木の高さの順に同型類を確定し、子の同型類ごとの出現回数で分割（partition）を疎に細分化する。
各親子辺は 1 度だけ個数の集計に寄与するため、比較ソート、連想配列、衝突し得るハッシュには依存しない。

## 空間計算量

根付き木の情報、分割、戻り値を含めて $O(N)$ である。

## 注意点

- 頂点番号と `root` は 0-indexed である。
- 空でない入力は無向の連結木でなければならない。端点の範囲、辺数、閉路、連結性、`root` に違反した場合は `std::runtime_error` を送出する。
- `class_id` の具体的な番号は同型関係だけを表し、永続的な識別子ではない。
