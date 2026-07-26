---
title: Vector Orthogonality (ベクトル直交判定)
documentation_of: ../../../../src/algorithm/geometry/2d/orthogonal_vectors.hpp
---

2本の二次元ベクトルが直交するか判定する。

## API

- `orthogonal(left, right)`: ベクトル `left` と `right` が直交すれば `true` を返す。

## API別の時間計算量・空間計算量

- `orthogonal`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

長さの積をscaleとする許容誤差付き内積判定を行う。零ベクトルを含む場合は互換性のため `true` を返す。
