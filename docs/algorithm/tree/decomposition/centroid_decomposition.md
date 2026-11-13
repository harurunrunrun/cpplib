---
title: Centroid Decomposition (重心分解)
documentation_of: ../../../../src/algorithm/tree/decomposition/centroid_decomposition.hpp
---

木の重心分解を行う。
元の木から重心を取り除く操作を再帰的に繰り返し、重心木を作る。

## API

- `CentroidDecomposition(graph)`: 隣接リスト `graph` で与えた木を重心分解する（構築時に `build` を呼ぶ）。
- `build()`: 重心分解をやり直す。
- `size()`: 頂点数を返す。
- `root()`: 重心木の根を返す。
- `parent(v)`: 重心木での親を返す。根では `-1`。
- `depth(v)`: 重心木での深さを返す。
- `tree()`: 重心木の隣接リストを返す。const 参照を返すため、列全体をコピーしない。

## 時間計算量

- `CentroidDecomposition(graph)`: `build` を含めて $O(N\log(N+1))$
- `build`: $O(N\log(N+1))$
- `size`, `root`, `parent`, `depth`, `tree`: $O(1)$

## 空間計算量

元の木と重心木、作業領域を含めて $O(N)$ である。

## 注意点

頂点は範囲内でなければならない。入力は無向の連結木とする。
端点の範囲外、閉路、辺数、連結性の違反時は `std::runtime_error` を送出する。
記載した計算量には木の検査を含む。
