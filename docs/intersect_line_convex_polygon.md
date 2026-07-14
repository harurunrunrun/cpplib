---
title: Line-Convex Polygon Intersection Test (直線と凸多角形の交差判定)
documentation_of: ../src/algorithm/geometry/2d/intersect_line_convex_polygon.hpp
---

## API

- `intersect_line_convex_polygon(line, polygon)`: 無限直線と閉凸集合が共通点を持つか判定する。

## 引数

`line` は異なる2点で定める直線。`polygon` は `NormalizedConvexPolygon` または弱凸な周回列である。

## 戻り値

接触を含め、共通点があれば `true` を返す。

## API別の時間計算量・空間計算量

- 正規化済みoverload: 時間 $O(\log N)$、追加領域 $O(1)$。
- 頂点列overload: 時間・領域 $O(N)$。

## 注意点

退化直線には `std::invalid_argument` を送出する。空の凸集合には `false` を返す。
退化判定に該当しない範囲では、直線の方向ベクトルを正の定数倍しても判定は変わらない。
