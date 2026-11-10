---
title: 3D Triangle Circumcircle (三次元三角形の外接円)
documentation_of: ../../../../../../src/algorithm/geometry/3d/circle/triangle/circumcircle.hpp
---

## API

- `circumcircle(triangle)`: 3 頂点を通り、三角形の平面上にある外接円を `Circle3` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

退化した三角形・非有限な入力には `std::invalid_argument` を送出する。円の法線は頂点順に従い、$(b-a)\times(c-a)$ の向きの単位ベクトルである。
退化判定と、外心の分子・分母は入力 `long double` の正確な 2 進有理数（dyadic）値で計算し、中心座標だけを最後に丸める。そのため、巨大な平行移動や一様に巨大・微小なスケールでも中間計算のオーバーフロー・アンダーフローを起こさない。中心または半径を `long double` で表現できない場合は `std::overflow_error` を送出する。
