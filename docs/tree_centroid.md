---
title: Tree Centroid
documentation_of: ../src/algorithm/tree/tree_centroid.hpp
---

木の重心。

重心は、取り除いたときに残る各連結成分のサイズがすべて $n/2$ 以下になる頂点。

## 関数

- `tree_centroids(graph)`
    - 重心をすべて返す。個数は 1 または 2。
- `tree_centroid(graph)`
    - 先頭の重心を返す。空木なら `-1`。

## 時間計算量

- `tree_centroids`: $O(N)$。木の検査と高々2頂点の出力を含む
- `tree_centroid`: $O(N)$。内部で `tree_centroids` を実行する

## 空間計算量

- 親、部分木size、走査順を含めて $O(N)$

## API契約・前提・例外

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
