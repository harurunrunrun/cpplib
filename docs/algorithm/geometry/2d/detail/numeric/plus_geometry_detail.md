---
title: Advanced Geometry Numeric Detail (高度幾何数値内部関数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/numeric/plus_geometry_detail.hpp
---

高度幾何ライブラリ間で共有する、scale付き符号、向き、距離、点包含、線分交差parameter、
多角形正規化の内部関数群。

## API

`sign`, `orientation`, `close`, `squared_distance`, `signed_area_twice`,
`point_in_polygon`, `segment_parameter`, `interpolate`, `normalized_polygon`,
`validate_finite` を提供する。

## 時間計算量

`point_in_polygon` と `normalized_polygon` は頂点数 $N$ に対して $O(N)$。
それ以外は $O(1)$。正規化の返り値を除く追加空間は $O(1)$。

## 注意点

公開algorithmを実装するためのdetail APIであり、許容誤差は `long double` のmachine epsilonに比例する。
