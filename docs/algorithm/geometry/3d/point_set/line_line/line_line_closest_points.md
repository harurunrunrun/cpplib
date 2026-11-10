---
title: Line-Line Closest Points (二直線の最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/line_line/line_line_closest_points.hpp
---

## API

- `closest_points(first, second)`: 2 直線間の距離を達成する点の対を、`first` 上の点、`second` 上の点の順の `std::pair` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 平行な場合など最近点対が複数あるときは、そのうち 1 組を返す。
- 退化直線や非有限入力には `std::invalid_argument` を送出する。
- 最近点を `long double` で表現できない場合は `std::overflow_error` を送出する。
