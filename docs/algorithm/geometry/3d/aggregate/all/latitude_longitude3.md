---
title: Latitude-Longitude Conversion 3D Umbrella (三次元緯度経度変換集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/latitude_longitude3.hpp
---

このヘッダは型の定義と、両方向の変換の個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。必要な変換の方向だけを使う場合は、`point3_from_latitude_longitude.hpp` または `latitude_longitude_from_point3.hpp` を直接 include すればよい。

## API

- `LatitudeLongitude3`: 緯度 `latitude`・経度 `longitude`・半径 `radius` を保持する（いずれも `long double`、デフォルトは 0）。角度の単位はラジアンである。
- `point3_from_latitude_longitude(latitude, longitude, radius = 1)`: 緯度・経度から `Point3` $(r \cos\varphi \cos\lambda, r \cos\varphi \sin\lambda, r \sin\varphi)$ に変換する（$\varphi$ は緯度、$\lambda$ は経度、$r$ は半径）。
- `latitude_longitude_from_point3(point)`: `Point3` から `LatitudeLongitude3` に変換する。緯度は $[-\pi/2, \pi/2]$、経度は `std::atan2` の値域 $[-\pi, \pi]$ で返す。極付近（方向ベクトルの $xy$ 成分がほぼ 0）では経度を 0 とする。

## API 別の時間計算量・空間計算量

- 各変換: 時間・追加空間ともに $O(1)$。
- 型の構築とメンバの参照: 時間・追加空間ともに $O(1)$。

## 注意点

- `point3_from_latitude_longitude`: 緯度・経度が非有限、半径が非有限または負、緯度が $[-\pi/2, \pi/2]$ から `GEOMETRY3D_EPS` を超えて外れている場合は `std::invalid_argument` を送出する（許容範囲内のはみ出しは $\pm \pi/2$ に丸める）。結果が非有限になれば `std::overflow_error` を送出する。
- `latitude_longitude_from_point3`: 点が非有限または原点なら `std::invalid_argument`、半径（原点からの距離）が `long double` で表現できなければ `std::overflow_error` を送出する。
