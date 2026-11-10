---
title: Latitude-Longitude from Point3 (三次元点から緯度経度への変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point/latitude_longitude_from_point3.hpp
---

原点を中心とする球面座標（緯度・経度・半径）へ三次元点を変換する。

## API

- `latitude_longitude_from_point3(point)`: 非零の `Point3` を `LatitudeLongitude3{latitude, longitude, radius}` に変換する。緯度は $[-\pi/2, \pi/2]$、経度は `std::atan2` の値域 $[-\pi, \pi]$ に入り、`radius` は原点からの距離である。極（$z$ 軸上の点）では経度を $0$ とする。

## API別の時間計算量・空間計算量

- `latitude_longitude_from_point3`: 時間・追加空間ともに $O(1)$。

## 注意点

- 有限な非零の点を与えること。非有限値または原点には `std::invalid_argument` を、半径の計算がオーバーフローした場合は `std::overflow_error` を送出する。
- 緯度・経度の単位はラジアンである。
