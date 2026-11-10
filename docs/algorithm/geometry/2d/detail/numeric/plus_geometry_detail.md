---
title: Advanced Geometry Numeric Detail (高度幾何の数値内部関数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/numeric/plus_geometry_detail.hpp
---

高度幾何ライブラリ間で共有する、スケール付き符号判定、向き、距離、点包含、線分交差のパラメータ、多角形の正規化を行う内部関数群。すべて名前空間 `plus_geometry_detail` に属する。

## API

- `scale(point)`: $\max(1, \lvert x \rvert, \lvert y \rvert)$ を返す。
- `sign(value, magnitude = 1)`: 許容幅 $128 \varepsilon \max(1, \mathrm{magnitude})$（$\varepsilon$ は `long double` の機械イプシロン）で `value` の符号を $-1, 0, 1$ で返す。
- `orientation(first, second, third)`: 3 点の向きを $-1, 0, 1$ で返す。
- `close(first, second)`: 2 点が許容誤差の範囲で一致するか。
- `squared_distance(first, second)`: 距離の 2 乗。
- `signed_area_twice(polygon)`: 符号付き面積の 2 倍。
- `point_in_polygon(polygon, point)`: 外部なら 0、境界上なら 1、内部なら 2 を返す。
- `segment_parameter(first, second, s, t)`: 2 線分が平行でなく交わるとき、各線分上の交点パラメータを `s`, `t` に書き込んで `true` を返す。
- `interpolate(segment, parameter)`: 線分上のパラメータ位置の点。
- `normalized_polygon(polygon)`: 末尾の閉じ点と連続する重複点を除き、反時計回りにそろえた多角形を返す。
- `validate_finite(point)`: 非有限座標なら `std::invalid_argument` を送出する。

## 時間計算量

`signed_area_twice`、`point_in_polygon`、`normalized_polygon` は頂点数 $N$ に対して $O(N)$、それ以外は $O(1)$。
正規化の返り値を除く追加領域は $O(1)$。

## 注意点

公開アルゴリズムを実装するための内部 API である。許容誤差は `GEOMETRY_EPS` ではなく、`long double` の機械イプシロンに比例する。
