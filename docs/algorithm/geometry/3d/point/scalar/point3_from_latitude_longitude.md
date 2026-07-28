---
title: Point3 from Latitude-Longitude (緯度経度から三次元点への変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/scalar/point3_from_latitude_longitude.hpp
---

## API

- `point3_from_latitude_longitude(latitude, longitude, radius=1)`: 緯度・経度・半径から、原点中心の直交座標 `Point3` を返す。

## API別の時間計算量・空間計算量

- `point3_from_latitude_longitude`: 時間・追加領域 $O(1)$。

## 注意点

角度単位はラジアン。緯度は $[-\pi/2,\pi/2]$、経度は任意の有限値、半径は有限な非負値でなければならない。不正入力には `std::invalid_argument`、計算結果のoverflowには `std::overflow_error` を送出する。
