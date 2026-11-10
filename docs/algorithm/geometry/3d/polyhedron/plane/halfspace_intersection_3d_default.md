---
title: 3D Halfspace Intersection without Interior Point (内点不要の三次元半空間交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/plane/halfspace_intersection_3d_default.hpp
---

閉半空間の共通部分を、狭義内点の探索も含めて有界凸多面体として構築する。

## API

- `halfspace_intersection_3d(halfspaces)`: 各 `Plane3{point, normal}` を、点 $p$ = `point` と法線 $n$ = `normal` に対する閉半空間 $n \cdot (x - p) \le 0$ と解釈する。共通部分が空集合なら空の `ConvexPolyhedron3`（`affine_dimension == -1`）を返し、非空かつ非有界なら `std::domain_error` を送出する。退化した（低次元の）有界な共通部分も返せる。

## API 別の時間計算量・空間計算量

制約数を $H$、出力サイズを $K$ とする。

- 狭義内点を持つ（full-dimensional な）非空の入力: 期待時間 $O(H \log H + K)$、期待追加領域 $O(H + K)$。
- 空集合・退化した入力・中間の射影が表現できない入力を処理する網羅的な経路: 最悪時間 $O(H^4 + K)$、追加領域 $O(H + K)$。三平面の全組を列挙し、各交点の実行可能性を判定する。

exact な整数演算は 1 回を定数時間と数える。

## 注意点

- 法線は非零、全座標は有限でなければならない。入力が不正な場合は `std::invalid_argument`、必要な結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
- 非有界な集合の半直線（ray）成分は返さない。
