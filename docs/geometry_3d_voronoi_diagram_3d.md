---
title: 3D Voronoi Diagram (三次元Voronoi図)
documentation_of: ../src/algorithm/geometry/3d/voronoi_diagram_3d.hpp
---

三次元Delaunay四面体分割の双対として、描画・包含処理に使えるVoronoi図を構築する。

## API

- `voronoi_diagram_3d(points)`: `long double` の3座標が完全一致する外心を
  最初の出現位置へ統合した有限頂点、cellの二等分halfspace、cyclic/path順ridge、
  有限segment、外向き無限rayと全incidenceを返す。完全一致する重複siteは
  除去される。

boundary Delaunay faceの法線は反対側の四面体頂点と逆向きに統一する。大座標差分と
crossは入力scaleで正規化してから計算する。

## API別の時間計算量・空間計算量

重複除去後のsite数を $N$、Delaunay四面体数を $T$ とする。有界cell $i$ の
halfspace数を $H_i$、halfspace intersectionで列挙する頂点数・凸包面数を
$V_i,F_i$ とし、有界cell集合を $\mathcal B$ とする。

| 処理 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| Delaunay構築 | $O(NT+N\log N)$ | $O(N+T)$ |
| 双対incidence構築 | $O(N+T\log T)$ | $O(N+T)$ |
| 有界cell $i$ の多面体構築 | $O(H_i^4+V_iF_i)$ | $O(H_i+V_i+F_i)$ |
| API全体 | $O(NT+N\log N+T\log T+\sum_{i\in\mathcal B}(H_i^4+V_iF_i))$ | $O(N+T+\sum_{i\in\mathcal B}(V_i+F_i))$ |

外心は座標をkeyとする辞書、Delaunay edgeとfaceのincidenceは直接索引で管理する。
したがって双対incidence構築に線形重複探索やedgeごとの全face走査による
$O(T^2)$ は発生しない。空間計算量は返却するcell多面体を含む。

## 注意点

- affine dimensionが3未満なら三次元双対の有限頂点・ridge・edgeは空にし、siteと
  空のcell recordだけを返す。lower-dimensional Voronoi図へ射影するAPIではない。
- 非有界cellでも `boundary_halfspaces` から包含判定できる。
- 有限cellの `bounded_polyhedron` は数値的に頂点列挙できた場合だけ設定するため、
  cell幾何の基本契約は常に存在する `boundary_halfspaces` である。
- 外心が有限範囲を超える場合は `std::overflow_error` を送出する。
- 四面体外心は入力座標のexact dyadic Cramer式で求め、最終座標だけを
  `long double`へ丸める。境界面と二等分面はanchorからの局所差分を正規化するため、
  巨大な共通移動量と非常に薄い局所形状を同時に扱える。
