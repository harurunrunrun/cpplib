---
title: Tree Centroid (木の重心)
documentation_of: ../../../../src/algorithm/tree/decomposition/tree_centroid.hpp
---

木の重心を求める。
重心とは、取り除いたときに残る各連結成分のサイズがすべて $n/2$ 以下になる頂点である。

## API

- `tree_centroids(graph)`: 重心をすべて返す。個数は 1 または 2 で、空木では空の列を返す。
- `tree_centroid(graph)`: 先頭の重心を返す。空木なら `-1` を返す。

## 時間計算量

- `tree_centroids`: 木の検査と高々 2 頂点の出力を含めて $O(N)$
- `tree_centroid`: 内部で `tree_centroids` を実行し、$O(N)$

## 空間計算量

親、部分木サイズ、走査順を含めて $O(N)$ である。

## 注意点

空木を除き、入力は無向の連結木とする。端点の範囲外、閉路、辺数、連結性の違反時は `std::runtime_error` を送出する。記載した計算量には木の検査を含む。
