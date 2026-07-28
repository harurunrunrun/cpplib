---
title: Planar Point Location (平面点位置検索)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/planar_point_location.hpp
---

互いに内部が交わらない多角形 face に対し、x-event sweep と path-copy AVL 木で永続 slab 検索木を構築する。

## API

- `PlanarPointLocationResult::outside` はどの face にも属さないことを表す番兵値。
- `PlanarPointLocationResult::face` は入力順の face 番号、`on_boundary` は境界上なら `true`。
- `PlanarPointLocation()` は空の検索器を作る。
- `PlanarPointLocation(faces)` は入力順の face 番号を保って前処理する。
- `locate(point)` は所属 face と境界フラグを返す。外部では `face == outside`。
- `face_count()` と `edge_count()` はそれぞれ格納した face 数と総辺数を返す。

## 時間計算量

全辺数を $V$ とする。各 event で永続 AVL 木の根までの経路だけを複製するため、構築は最悪
$O(V\log V)$ 時間・空間、`locate` は最悪 $O(\log V)$ 時間。件数取得は $O(1)$。

## 注意点

各 face は正の面積を持つ非退化単純多角形で、face 内部は互いに交差してはならない。面積0は `invalid_argument`。共有辺・共有頂点は許す。単純性と入力全体の planarity 自体は検査しない。
境界上ではその境界を持つ face の一つを返す。座標は有限値でなければならない。
