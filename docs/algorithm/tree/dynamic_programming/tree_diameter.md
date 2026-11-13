---
title: Tree Diameter (木の直径) [GRL_5_A] [tree_diameter]
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/tree_diameter.hpp
---

木の直径を求め、両端点、長さ、端点間のパスを返す。

## API

- `TreeDiameterResult tree_diameter(const std::vector<std::vector<int>>& graph)`: 重みなし木の直径を返す。長さは辺数（`long long`）である。
- `WeightedTreeDiameterResult<T> weighted_tree_diameter<T>(const std::vector<std::vector<std::pair<int, T>>>& graph)`: 重み付き木の直径を返す。隣接リストの各要素は `(隣接頂点, 辺の重み)` である。長さの型は `T` である。

両方の結果型はメンバ `from`, `to`, `length`, `path` を持つ。`path` は `from` から `to` へのパスで、両端を含む。
空木では端点が `-1`、長さが $0$、`path` が空になる。

## 時間計算量

$L$ を返す直径のパスの頂点数とする。

- `tree_diameter`: $O(N+L)=O(N)$
- `weighted_tree_diameter`: $O(N+L)=O(N)$

どちらも、木の検査、2 回の全頂点の走査、パスの復元を含む。

## 空間計算量

距離・親・スタックと出力するパスを含めて $O(N)$ である。

## 注意点

- 重み付き版は、辺の重みが非負であることを前提とする。
- 空木を除き、入力は無向の連結木とする。端点の範囲外、閉路、辺数、連結性の違反時は `std::runtime_error` を送出する。記載した計算量には木の検査を含む。
