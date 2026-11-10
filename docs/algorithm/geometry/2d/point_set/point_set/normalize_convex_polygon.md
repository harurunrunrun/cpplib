---
title: Normalize Convex Polygon (凸多角形の正規化)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set/normalize_convex_polygon.hpp
---

## API

- `std::vector<Point> normalize_convex_polygon(std::vector<Point> polygon)`: 弱凸な周回列を、反時計回りかつ $(x, y)$ の辞書順で最小の頂点から始まる列に正規化する。

連続する重複頂点、閉路として末尾に繰り返された先頭頂点、中間の共線点を除く。全点が共線なら両端の 2 点だけ（全点が一致するならその 1 点）を返す。

## API別の時間計算量・空間計算量

- `normalize_convex_polygon`: 頂点数を $N$ として、入力の検証を含めて時間 $O(N)$、追加領域 $O(N)$。

## 注意点

入力は、時計回りまたは反時計回りに境界を一周する弱凸な周回列でなければならない。次のような列には `std::invalid_argument` を送出する。

- 左折と右折が混在する列
- 辺の方向が一周を超えて回転する自己交差した星形
- 同じ輪郭を複数周する列
