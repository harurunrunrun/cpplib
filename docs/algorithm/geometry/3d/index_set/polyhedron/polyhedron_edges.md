---
title: General Polyhedron Geometric Edges (一般多面体の幾何的な辺)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/polyhedron_edges.hpp
---

凸性を仮定せず、三角形分割の内部の対角線を除いた幾何的な辺を、頂点添字の組で返す。

## API

- `polyhedron_edges(polyhedron)`: 各辺を `{min_index, max_index}` として一度ずつ返す。出力は辞書順である。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$、出力の辺数を $E$ として、基数ソートを用いるため時間 $O(V+F)$、追加領域 $O(V+F+E)$。

## 注意点

共有辺の両側の三角形が同一平面上にあれば、その辺を三角形分割の内部の対角線として除く（判定は正確な `adaptive_orient3d`）。3 面以上が共有する辺には `std::domain_error` を送出する。
入力は `geometry3d_validate` で検査し、非有限な頂点や同じ頂点を繰り返す面には `std::invalid_argument`、範囲外の頂点添字には `std::out_of_range` を送出する。
