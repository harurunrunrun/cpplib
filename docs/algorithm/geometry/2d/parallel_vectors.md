---
title: Vector Parallelism (ベクトル平行判定)
documentation_of: ../../../../src/algorithm/geometry/2d/parallel_vectors.hpp
---

2本の二次元ベクトルが平行か判定する。

## API

- `parallel(left, right)`: ベクトル `left` と `right` が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

長さの積をscaleとする許容誤差付き外積判定を行う。零ベクトルを含む場合は互換性のため `true` を返す。
