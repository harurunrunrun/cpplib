---
title: 3D Line-Line Skewness (三次元直線同士のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_line/line_line_skew.hpp
---

## API

- `skew(first, second)`: 2本の `Line3` が非平行かつ交点を持たないなら `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

平行な異なる直線と一致する直線はいずれも `false`。退化直線には `std::invalid_argument` を送出する。
