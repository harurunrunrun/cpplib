---
title: 3D Delaunay Tetrahedralization (三次元Delaunay四面体分割)
documentation_of: ../src/algorithm/geometry/3d/delaunay_tetrahedralization_3d.hpp
---

Bowyer--Watson増分法で有限点集合のDelaunay四面体分割を構築する。包含球判定には
`adaptive_insphere`、向き・最終境界判定には `adaptive_orient3d` を使う。

## API

- `delaunay_tetrahedralization_3d(points)`: 完全一致する重複点を除き、
  `DelaunayTetrahedralization3` を返す。共球面では厳密に球内部の四面体だけを空洞から
  削除するため、可能なDelaunay分割の一つを決定的に返す。

super tetraを除いた後、全入力点が使われ、全boundary faceが入力凸包のsupporting
faceであることを適応的述語で検査する。有限super tetraの影響が残った場合はscaleを
拡大して再構築する。

## API別の時間計算量・空間計算量

生成途中を含む四面体数を $T$ として、1回の構築は時間 $O(NT+N\log N)$、境界検査は
$O(NT)$、追加領域は $O(T+N)$。三次元Delaunay分割の出力は最悪 $O(N^2)$ のため
最悪時間は $O(N^3)$。super tetra再構築回数は12以下の定数。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 共線・共面入力も受理するが、三次元四面体は生成しない。
- 重複判定は許容誤差ではなく座標の完全一致で行う。
- 位相判定は適応的精度で行う。表現可能なsuper tetraで境界を覆えないほど
  ill-conditionedな入力では `std::overflow_error` を送出する。
- affine dimensionはexact dyadic predicateで判定する。構築座標は先頭点をanchorとする
  局所差分を共通scaleへ正規化するため、巨大な共通移動量と薄い局所形状を同時に扱える。
