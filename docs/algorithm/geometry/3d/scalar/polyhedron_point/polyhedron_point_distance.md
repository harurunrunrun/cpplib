---
title: General Polyhedron Point Distance (一般多面体と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron_point/polyhedron_point_distance.hpp
---

## API

- `polyhedron_point_distance(polyhedron, point)`: 凸性を仮定しない多面体の閉領域と点の最小ユークリッド距離を返す。点が内部または境界上にあれば $0$ を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間 $O(V + F \log F)$、追加領域 $O(V + F)$。
- 内部判定ののち、呼び出しごとに面の AABB 木を構築（$O(F \log F)$）し、優先度付きキューで最近面を探索する。

## 注意点

- 入力の前提と例外は `polyhedron_closest_point` に従う（面が空の多面体、非有限な入力、不正な面には `std::invalid_argument`、範囲外の頂点添字には `std::out_of_range`）。
- 距離を `long double` で表現できない場合は `std::overflow_error` を送出する。
