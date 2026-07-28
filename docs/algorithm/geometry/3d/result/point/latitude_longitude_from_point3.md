---
title: Latitude-Longitude from Point3 (三次元点から緯度経度への変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point/latitude_longitude_from_point3.hpp
---

## API

- `latitude_longitude_from_point3(point)`: 非零の `Point3` を `LatitudeLongitude3` に変換する。極では経度を $0$ とする。

## API別の時間計算量・空間計算量

- `latitude_longitude_from_point3`: 時間・追加領域 $O(1)$。

## 注意点

有限な非零点を与える。非有限値または原点には `std::invalid_argument`、半径計算のoverflowには `std::overflow_error` を送出する。緯度・経度の単位はラジアンである。
