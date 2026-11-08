---
title: Closest Point on 3D Primitive (三次元図形上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/closest_point.hpp
---

## API

- `closest_point(ray, point)`: 半直線上で `point` に最も近い点を返す。
- `closest_point(segment, point)`: 閉線分上で `point` に最も近い点を返す。
- `closest_point(triangle, point)`: 閉三角形上で `point` に最も近い点を返す。

いずれも戻り値は `Point3` である。

## API 別の時間計算量・空間計算量

- 各オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した線分（両端点が一致するもの）には対応する。
- 退化した半直線・退化した三角形・非有限な入力には `std::invalid_argument` を送出する。
- 半直線と線分は共通の局所座標ソルバで、三角形は辺のスケールで正規化した Voronoi 領域判定で計算し、FMA を用いて最近点を復元する。
- 結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
