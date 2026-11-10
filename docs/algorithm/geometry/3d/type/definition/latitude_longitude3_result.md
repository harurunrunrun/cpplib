---
title: Latitude-Longitude 3D Result (三次元緯度経度変換結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/latitude_longitude3_result.hpp
---

## API

- `LatitudeLongitude3::latitude`: 緯度（ラジアン）。
- `LatitudeLongitude3::longitude`: 経度（ラジアン）。
- `LatitudeLongitude3::radius`: 原点からの距離。

## API別の時間計算量・空間計算量

- 構築と各メンバへのアクセス: 時間・追加領域 $O(1)$。

## 注意点

- この型自体は値を検査しない。変換関数 `latitude_longitude_from_point3` が返す値では、緯度は $[-\pi/2, \pi/2]$、経度は $[-\pi, \pi]$ の範囲にあり、極（$z$ 軸上の点）の経度は $0$ になる。
