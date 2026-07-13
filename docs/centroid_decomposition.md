---
title: Centroid Decomposition
documentation_of: ../src/algorithm/tree/centroid_decomposition.hpp
---

木の重心分解。

元の木から重心を取り除く操作を再帰的に行い、重心木を作る。

## 関数

- `CentroidDecomposition(graph)`
- `root()`
    - 重心木の根。
- `parent(v)`
    - 重心木での親。根は `-1`。
- `depth(v)`
    - 重心木での深さ。
- `tree()`
    - 重心木の隣接リスト。

## 時間計算量

- `CentroidDecomposition(tree)`: `build` を含めて $O(N\log(N+1))$
- `build`: $O(N\log(N+1))$
- `size`, `root`, `parent`, `depth`, `tree`: $O(1)$

`tree()` は重心木の隣接リストへのconst referenceを返すため、列全体をコピーしない。

## 空間計算量

- 元の木と重心木、作業領域を含めて $O(N)$

## API契約・前提・例外

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
