---
title: Latitude-Longitude Conversion 3D (三次元緯度経度変換)
documentation_of: ../src/algorithm/geometry/3d/latitude_longitude3.hpp
---

## API

- `LatitudeLongitude3`: `latitude`, `longitude`, `radius`を保持する。
- `point3_from_latitude_longitude(latitude, longitude, radius=1)`: 緯度・経度・半径から原点中心の直交座標を返す。
- `latitude_longitude_from_point3(point)`: 非零点を緯度$[-\pi/2,\pi/2]$、経度$[-\pi,\pi]$、半径へ変換する。極では経度を0とする。

## API別の時間計算量・空間計算量

各変換は時間$O(1)$、追加領域$O(1)$。

## 注意点

角度単位はラジアンで、緯度は$[-\pi/2,\pi/2]$、半径は有限な非負値でなければならない。経度は任意の有限値を受け付け周期的に扱う。直交座標からの変換では原点を拒否する。
