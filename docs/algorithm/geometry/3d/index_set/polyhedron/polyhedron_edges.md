---
title: General Polyhedron Geometric Edges (一般多面体の幾何的な辺)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/polyhedron_edges.hpp
---

凸性を仮定せず、三角形分割の内部対角線を除いた幾何的な辺を頂点index対で返す。

## API

- `polyhedron_edges(polyhedron)`: 各辺を `{min_index, max_index}` で一度だけ返す。出力は辞書順。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$、出力辺数を $E$ として時間計算量 $O(V+F)$、追加空間計算量 $O(V+F+E)$。

## 注意点

共有辺の両側の三角形が同一平面なら、その辺を三角形分割の内部対角線として除く。3面以上が共有する辺には `std::domain_error` を送出する。
