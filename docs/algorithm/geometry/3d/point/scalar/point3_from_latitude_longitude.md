---
title: Point3 from Latitude-Longitude (緯度経度から三次元点への変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/scalar/point3_from_latitude_longitude.hpp
---

## API

- `point3_from_latitude_longitude(latitude, longitude, radius = 1)`: 緯度・経度・半径から、原点中心の直交座標 `Point3` を返す。戻り値は $(r \cos\varphi \cos\lambda,\; r \cos\varphi \sin\lambda,\; r \sin\varphi)$ である（$\varphi$ は緯度、$\lambda$ は経度、$r$ は半径）。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 角度の単位はラジアンである。
- 緯度は $[-\pi/2, \pi/2]$、経度は任意の有限値、半径は有限な非負値でなければならない。緯度は `GEOMETRY3D_EPS` だけ範囲外まで許容し、範囲内に丸めて用いる。
- 不正な入力には `std::invalid_argument`、計算結果のオーバーフローには `std::overflow_error` を送出する。
