---
title: Minimum Enclosing Ellipse (最小包含楕円)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_ellipse.hpp
---

Khachiyan反復で最小体積包含楕円を指定許容誤差まで近似する。

## API

`minimum_enclosing_ellipse(points, tolerance, maximum_iterations)` は中心、二次形式、
近似許容誤差を返す。既定値は `1e-4` と `200000`。

## 時間計算量

点数を $N$、実行反復数を $I\le maximum_iterations$ とすると $O(NI)$ 時間、$O(N)$ 空間。
返却前の包含補正は $O(N)$。

## 注意点

返却前に全点の二次形式最大値でformを縮小するため、有限丸めの範囲で全入力点を必ず包含する。
反復上限では `runtime_error`、2次元で退化した点集合では `domain_error`。
