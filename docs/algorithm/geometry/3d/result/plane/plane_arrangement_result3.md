---
title: PlaneArrangement3 (三次元平面アレンジメント結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane/plane_arrangement_result3.hpp
---

平面アレンジメントについて、重複除去済みの平面、交線、三面交点、三次元領域数を保持する構造体 `PlaneArrangement3` を提供する。

## API

- `planes`（`std::vector<Plane3>`）: 幾何的に一致する平面を 1 つにまとめた列。
- `intersection_lines`（`std::vector<Line3>`）: 2 平面が定める相異なる交線。
- `intersection_points`（`std::vector<Point3>`）: 3 平面が定める相異なる交点。
- `region_count`（`std::uint64_t`、デフォルト値 $1$）: 平面群が三次元空間を分割してできる連結領域の数。

## API別の時間計算量・空間計算量

各メンバへのアクセスは $O(1)$、列の走査は要素数に比例する。平面数を $P$、相異なる交線数を $L$、交点数を $Q$ とすると、格納領域は $O(P+L+Q)$。

## 注意点

- `region_count` は境界上の低次元セルの数ではなく、三次元領域の数である。
- 交線は、方向や基準点が異なっていても幾何的に一致すれば 1 つにまとめる。
