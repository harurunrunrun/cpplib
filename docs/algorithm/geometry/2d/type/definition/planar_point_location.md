---
title: Planar Point Location (平面点位置検索)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/planar_point_location.hpp
---

内部が互いに交わらない多角形の面（face）の集まりに対し、$x$ 座標のイベントによる掃引と経路複製（path copying）による永続 AVL 木で、永続的なスラブ検索木を構築する。

## API

- `PlanarPointLocationResult::outside` は、どの面にも属さないことを表す番兵値。
- `PlanarPointLocationResult::face` は入力順の面番号、`on_boundary` は境界上なら `true`。
- `PlanarPointLocation()` は空の検索器を作る。
- `explicit PlanarPointLocation(std::vector<std::vector<Point>> faces)` は、入力順の面番号を保ったまま前処理する。各面は内部で正規化される。
- `locate(point)` は点が属する面と境界フラグを返す。どの面にも属さなければ `face == outside`。
- `face_count()` と `edge_count()` は、それぞれ格納した面の数と（正規化後の）総辺数を返す。

## 時間計算量

全辺数を $V$ とする。各イベントで永続 AVL 木の根までの経路だけを複製するため、構築は最悪
$O(V \log V)$ 時間・空間、`locate` は最悪 $O(\log V)$ 時間。件数の取得は $O(1)$。

## 注意点

- 各面は面積が正の非退化な単純多角形で、面の内部どうしが交差してはならない。共有辺・共有頂点は許す。単純性と入力全体の平面性そのものは検査しない。
- 正規化後の頂点数が $3$ 未満の面、面積が $0$ の面、非有限な座標には `std::invalid_argument` を送出する。`locate` の点も有限でなければならない。
- 境界上の点に対しては、その境界をもつ面のうちいずれか 1 つを返す。
