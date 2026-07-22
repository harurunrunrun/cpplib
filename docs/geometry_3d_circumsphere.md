---
title: 3D Tetrahedron Circumsphere (三次元四面体の外接球)
documentation_of: ../src/algorithm/geometry/3d/shape/circumsphere.hpp
---

## API

- `circumsphere(tetrahedron)`: 4頂点を通る外接球を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

4頂点が共面な退化四面体・非有限入力には `std::invalid_argument` を送出する。
退化判定とCramer式は入力 `long double` のexact dyadic値で計算し、中心座標だけを
最後に丸める。巨大な平行移動や一様な巨大・微小scaleでも中間overflow・underflowを
避ける。中心または半径を表現できない場合は `std::overflow_error` を送出する。
