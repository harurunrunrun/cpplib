---
title: 3D Triangle Circumcircle (三次元三角形の外接円)
documentation_of: ../src/algorithm/geometry/3d/shape/circumcircle.hpp
---

## API

- `circumcircle(triangle)`: 3頂点を通り三角形の平面上にある外接円を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化三角形・非有限入力には `std::invalid_argument` を送出する。円の法線は頂点順に従う。
退化判定、外心の分子・分母は入力 `long double` のexact dyadic値で計算し、中心座標
だけを最後に丸める。巨大な平行移動や一様な巨大・微小scaleでも中間overflow・underflowを
避ける。中心または半径を表現できない場合は `std::overflow_error` を送出する。
