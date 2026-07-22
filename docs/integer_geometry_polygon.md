---
title: Exact Integer Polygon Operations (厳密整数多角形操作)
documentation_of: ../src/algorithm/integer_geometry/polygon.hpp
---

## API

- `polygon_twice_signed_area(polygon)`: 頂点順に符号付き面積の $2$ 倍を返す。反時計回りなら正、時計回りなら負。
- `polygon_twice_area(polygon)`: 面積の $2$ 倍の絶対値を返す。
- `point_in_polygon(polygon, point)`: 単純多角形に対する点の外部・境界・内部を返す。
- `lattice_boundary_points(polygon)`: 多角形の境界上の格子点数を返す。
- `lattice_interior_points(polygon)`: Pickの定理により内部格子点数を返す。
- `lattice_point_count(polygon)`: 境界・内部格子点数をまとめて返す。
- `LatticePointCount`: `boundary` と `interior` を持つ格子点数の結果型。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。すべて時間 $O(N\log C)$、追加領域 $O(1)$。面積・点包含だけなら時間 $O(N)$。$C$ は隣接頂点の座標差であり、$\log C$ は境界点数計算の最大公約数に由来する。

## 注意点

頂点列は時計回りでも反時計回りでもよい。点包含とPickの定理には自己交差しない多角形を渡す。Pickの定理には頂点が格子点で面積が正の多角形が必要である。頂点数・面積・偶奇関係から検出できる前提違反には `std::invalid_argument` を送出するが、自己交差性の検査は行わない。外積、面積、境界点数の途中または結果が128 bitの戻り値型で表せない場合は `std::overflow_error` を送出する。
