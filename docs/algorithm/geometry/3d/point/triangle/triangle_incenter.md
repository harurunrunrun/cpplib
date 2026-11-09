---
title: 3D Triangle Incenter (三次元三角形の内心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/triangle/triangle_incenter.hpp
---

## API

- `triangle_incenter(triangle)`: 3 辺から等距離にある内心を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 退化三角形や非有限入力には `std::invalid_argument` を送出する。退化判定は exact dyadic 演算による厳密な述語で行う。
- 辺の差分を共通の局所スケールへ正規化して辺長と重みを求め、内心を FMA で頂点 `a` 基準の元の座標へ復元する。
- 局所的な形状または結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
