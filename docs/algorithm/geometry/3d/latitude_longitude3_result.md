---
title: Latitude-Longitude 3D Result (三次元緯度経度変換結果)
documentation_of: ../../../../src/algorithm/geometry/3d/latitude_longitude3_result.hpp
---

## API

- `LatitudeLongitude3::latitude`: 緯度をラジアンで保持する。
- `LatitudeLongitude3::longitude`: 経度をラジアンで保持する。
- `LatitudeLongitude3::radius`: 原点からの距離を保持する。

## API別の時間計算量・空間計算量

- 構築と各fieldへのアクセス: 時間・追加領域 $O(1)$。

## 注意点

この型自体は値を検査しない。変換関数が返す値では緯度は $[-\pi/2,\pi/2]$、経度は $[-\pi,\pi]$ で、極の経度は $0$ になる。
