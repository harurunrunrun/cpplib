---
title: Orient Polygon3 (三次元多角形の向き統一)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polygon/polygon_point/orient_polygon3.hpp
---

## API

- `orient_polygon3(polygon, reference_normal)`: 先頭頂点を保ったまま、`reference_normal` の方向から見て反時計回りになる頂点順の多角形を返す。符号付き面積が負なら 2 番目以降の頂点を逆順にする。

## API 別の時間計算量・空間計算量

頂点数を $N$ として、時間 $O(N)$、戻り値を含む領域 $O(N)$。

## 注意点

零の参照法線、または 3 頂点未満の多角形には `std::invalid_argument` を送出する。
