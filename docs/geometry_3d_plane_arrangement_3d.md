---
title: 3D Plane Arrangement (三次元平面アレンジメント)
documentation_of: ../src/algorithm/geometry/3d/plane_arrangement_3d.hpp
---

平面を順に挿入し、新しい平面上に既存平面が作る直線アレンジメントの領域数を加えて
三次元領域数を求める。同時に相異なる交線と三面交点を列挙する。

## API

- `plane_arrangement_3d(planes)`: `PlaneArrangement3` を返す。一致平面は入力順に
  関係なく一度だけ数え、平行平面と多重交点も領域数へ正しく反映する。

## API別の時間計算量・空間計算量

平面数を $P$ とする。領域数計算は $O(P^3)$。交点の線形重複除去まで含む単純な
全列挙の最悪時間は $O(P^6)$、追加領域は交線・交点の出力を含め $O(P^3)$。

## 注意点

- 法線は非零、点と法線の全座標は有限でなければならない。違反時は
  `std::invalid_argument` を送出する。
- 領域数が `std::uint64_t` を超える場合は `std::overflow_error` を送出する。
