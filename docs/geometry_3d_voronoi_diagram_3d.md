---
title: 3D Voronoi Diagram (三次元Voronoi図)
documentation_of: ../src/algorithm/geometry/3d/voronoi_diagram_3d.hpp
---

三次元Delaunay四面体分割の双対として、描画・包含処理に使えるVoronoi図を構築する。

## API

- `voronoi_diagram_3d(points)`: `long double` の3座標が完全一致する外心を
  最初の出現位置へ統合した有限頂点、cellの二等分halfspace、cyclic/path順ridge、
  有限segment、外向き無限rayと全incidenceを返す。完全一致する重複siteは
  除去される。Delaunay構築には固定seedを使い、同じ入力には同じ分割を返す。
- `voronoi_diagram_3d_randomized(points, seed)`: 指定した `seed` のランダム挿入順で
  Delaunay分割を構築し、同じ形式のVoronoi図を返す。同じ入力とseedには決定的である。
- `VoronoiDiagram3::sites`, `finite_vertices`: 重複除去済みsiteと、Delaunay四面体に
  双対な有限Voronoi頂点を保持する。
- `VoronoiDiagram3::cells`: 各siteのcellを保持する。`neighbors` と
  `ridge_indices` と `boundary_halfspaces` の同じ添字が同じ隣接cell境界を表す。
  有界cellの `bounded_polyhedron` はridgeの巡回頂点列から直接三角形分割される。
- `VoronoiDiagram3::ridges`: Delaunay edgeに双対な二次元境界を保持する。有界ridgeの
  `finite_vertices` は巡回順、非有界ridgeでは有限部分のpath順である。
- `VoronoiDiagram3::edges`: Delaunay faceに双対な一次元境界を保持する。
  `segment` または外向き `ray` のいずれかを持つ。共球退化では一点だけを持つ
  長さ0の有限edgeもある。

boundary Delaunay faceの法線は反対側の四面体頂点と逆向きに統一する。大座標差分と
crossは入力scaleで正規化してから計算する。

## API別の時間計算量・空間計算量

重複除去後のsite数を $N$、Delaunay四面体数を $T$、返す頂点・edge・ridge・cell面と
それらのincidence総数を $K$ とする。Delaunay増分中に生成する四面体と保持する
conflict関係の総数を $D$、検査するconflict組数を $C$ とする。
hash tableの期待計算量を用いる。$T\le D$ である。

| 処理 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 固定seed APIのDelaunay構築 | $O(N\log N+D+C)$ | $O(N+D)$ |
| randomized APIのDelaunay構築（一般位置・期待） | $O(N\log N+D)$ | $O(N+D)$ |
| 双対incidenceと全有界cell面の直接構築（期待） | $O(T+K)$ | $O(T+K)$ |
| `voronoi_diagram_3d` 全体（期待hash） | $O(N\log N+D+C+K)$ | $O(N+D+K)$ |
| `voronoi_diagram_3d_randomized` 全体（一般位置・期待） | $O(N\log N+D+K)$ | $O(N+D+K)$ |

外心、Delaunay edge、Delaunay faceはhash索引で一度だけ集約する。有界cellの面は
双対ridgeの巡回列をfan分割するため、cellごとの高次数halfspace intersectionを
実行しない。三次元Delaunay/Voronoiの出力自体は最悪 $K=O(N^2)$ になり得る。

## 注意点

- affine dimensionが3未満なら三次元双対の有限頂点・ridge・edgeは空にし、siteと
  空のcell recordだけを返す。lower-dimensional Voronoi図へ射影するAPIではない。
- 非有界cellでも `boundary_halfspaces` から包含判定できる。
- 有界cellの `bounded_polyhedron` は双対incidenceから構築する。共球なDelaunay
  補助edgeに双対な面積0のridgeはcell面から除外するが、incidence recordには残す。
- hash tableを用いるため、edge・ridgeの格納順はAPI契約に含まれない。接続は必ず
  添字fieldを使って参照する。
- 外心が有限範囲を超える場合は `std::overflow_error` を送出する。
- 四面体外心は入力座標のexact dyadic Cramer式で求め、最終座標だけを
  `long double`へ丸める。境界面と二等分面はanchorからの局所差分を正規化するため、
  巨大な共通移動量と非常に薄い局所形状を同時に扱える。
