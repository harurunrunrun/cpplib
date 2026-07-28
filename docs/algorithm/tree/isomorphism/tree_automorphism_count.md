---
title: Tree Automorphism Count (木の自己同型数)
documentation_of: ../../../../src/algorithm/tree/isomorphism/tree_automorphism_count.hpp
---

子部分木の同型類と同型な子の置換数から、木の自己同型写像数を数える。

## API

- `rooted_tree_automorphism_count<T>(graph, root)`: 根を固定する自己同型数を `T` で返す。
- `tree_automorphism_count<T>(graph)`: 根なし木の自己同型数を返す。

## 時間計算量

- いずれも $O(N\log N)$。

## 注意点

- `T` は加算・乗算と整数からの構築を備える型とする。法整数型も利用できる。
- 入力は単純な無向木とする。
