---
title: 2D Ray (二次元半直線)
documentation_of: ../src/algorithm/geometry/2d/core/ray.hpp
---

## API

`Ray{origin, through}`

## 引数

`origin` は始点、`through` は進行方向上の点である。

## 戻り値

公開field `origin`, `through` を持つaggregate型である。

## API別の時間計算量・空間計算量

- 構築: 時間 $O(1)$、領域 $O(1)$。

## 注意点

半直線を使う演算では `origin != through` が必要である。
