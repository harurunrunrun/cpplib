---
title: Latitude-Longitude Conversion 3D Umbrella (三次元緯度経度変換集約)
documentation_of: ../../../../src/algorithm/geometry/3d/latitude_longitude3.hpp
---

## API

- `LatitudeLongitude3`: 緯度・経度・半径を保持する。
- `point3_from_latitude_longitude(latitude, longitude, radius=1)`: 緯度経度から `Point3` へ変換する。
- `latitude_longitude_from_point3(point)`: `Point3` から `LatitudeLongitude3` へ変換する。
- このヘッダは型定義と両変換方向のleafをまとめてincludeする後方互換aggregatorである。

## API別の時間計算量・空間計算量

- 各変換: 時間・追加領域 $O(1)$。
- 型の構築とfieldアクセス: 時間・追加領域 $O(1)$。

## 注意点

必要な変換方向だけを使う場合は `point3_from_latitude_longitude.hpp` または `latitude_longitude_from_point3.hpp` を直接includeできる。
