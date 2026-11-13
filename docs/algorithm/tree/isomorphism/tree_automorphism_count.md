---
title: Tree Automorphism Count (木の自己同型数)
documentation_of: ../../../../src/algorithm/tree/isomorphism/tree_automorphism_count.hpp
---

子の部分木の同型類と、同型な子どうしの置換の数から、木の自己同型写像の個数を数える。

## API

- `rooted_tree_automorphism_count<T>(graph, root)`: 根を固定する自己同型の個数を `T` で返す。
- `tree_automorphism_count<T>(graph)`: 根なし木の自己同型の個数を返す。中心が 2 つある場合は、その間に仮想的な頂点を挿入した木を根付きで数える。空木では `T{1}` を返す。

## 時間計算量

いずれも $O(N\log N)$ である。

## 注意点

- `T` は乗算と整数からの構築を備える型とする。法を取る整数型も利用できる。
- 入力は単純な無向木とする。
- `root` や隣接リスト中の頂点番号が範囲外なら `std::out_of_range` を送出する。
- `tree_automorphism_count` は、入力が木でない（辺数が $N-1$ でない、または非連結）なら `std::invalid_argument` を送出する。`rooted_tree_automorphism_count` は、根から到達できない頂点があれば `std::invalid_argument` を送出する。
