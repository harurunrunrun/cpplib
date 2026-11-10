---
title: 3D Principal Axes (三次元主成分軸)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/principal_axes_3d.hpp
---

点集合の重心と母共分散行列を計算し、分散の降順に 3 本の主成分軸を返す。

## API

- `principal_axes_3d(points, relative_epsilon = GEOMETRY3D_EPS)`: 点集合の母共分散行列を固有値分解し、`PrincipalAxes3{centroid, axes, variances}` を返す。`axes[i]` は単位固有ベクトル、`variances[i]` はその方向の分散で、`variances[0] >= variances[1] >= variances[2]` の順に並ぶ。全点が原点にある場合は、軸を座標軸、分散を $0$ として返す。

## 時間計算量

点数を $N$ とすると時間 $O(N)$、戻り値を除く追加空間 $O(1)$。

## 注意点

- 共分散は $N$ で割る母分散として定義する。
- 空集合、非有限な点、負・非有限の `relative_epsilon` には `std::invalid_argument` を送出する。
- 分散が `long double` で表現できない場合は `std::overflow_error` を送出する。
- 絶対値が `relative_epsilon` 程度以下の負の固有値は $0$ に丸める。
- 重複した分散に対応する軸は一意ではない。
