---
title: 3D Plane Arrangement (三次元平面アレンジメント)
documentation_of: ../src/algorithm/geometry/3d/shape/plane_arrangement_3d.hpp
---

平面を順に挿入し、新しい平面上に既存平面が作る直線アレンジメントの領域数を加えて
三次元領域数を求める。同時に相異なる交線と三面交点を列挙する。

## API

- `plane_arrangement_3d(planes)`: `PlaneArrangement3` を返す。一致平面は入力順に
  関係なく一度だけ数え、平行平面と多重交点も領域数へ正しく反映する。

## API別の時間計算量・空間計算量

平面数を $P$ とする。

| API・処理 | 時間計算量 | 空間計算量（出力を含む） |
| --- | --- | --- |
| 平面の正規化・一致平面の除去 | $O(P^2)$ | $O(P)$ |
| 領域数の計算 | $O(P^3\log P)$ | $O(P^2)$ |
| 三面交点の列挙・重複除去 | $O(P^3\log P)$ | $O(P^3)$ |
| `plane_arrangement_3d` 全体 | $O(P^4+P^3\log P)=O(P^4)$ | $O(P^3)$ |

全体の $O(P^4)$ は、許容誤差付きで一致する交線を入力順に重複除去する処理の
最悪計算量による。三面交点の重複除去は空間ハッシュで行うため、従来の線形探索に
よる $O(P^6)$ は発生しない。

## 注意点

- 法線は非零、点と法線の全座標は有限でなければならない。違反時は
  `std::invalid_argument` を送出する。
- 領域数が `std::uint64_t` を超える場合は `std::overflow_error` を送出する。
