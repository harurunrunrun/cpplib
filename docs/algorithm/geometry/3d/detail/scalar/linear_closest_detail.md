---
title: 3D Linear Closest Detail (三次元線形最近点内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/scalar/linear_closest_detail.hpp
---

## API

名前空間 `geometry3d_linear_closest_detail` に次の要素を置く。

- `Kind`, `Input`: 直線（`Kind::line`）・半直線（`Kind::ray`）・線分（`Kind::segment`）を 2 点と種別で表す内部表現。
- `query(first, second)`: 直線・半直線・線分の組に対する最近点対を、この内部表現から求める。
- `checked_distance(first, second)`, `restore(local, anchor, scale)`: 正規化した座標と元のスケールの間を、オーバーフロー検査付きで変換する。

## API別の時間計算量・空間計算量

- 全 API: 時間・追加領域ともに $O(1)$。

## 注意点

公開 API ではない。`query` は非有限な点、および退化した直線・半直線に `std::invalid_argument` を送出する（退化した線分は許す）。結果の距離・座標を `long double` で表現できない場合は `std::overflow_error` を送出する。
