---
title: Polygon3 Triangulation Indices (三次元多角形の頂点番号三角形分割)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polygon/polygon3_triangulation_indices.hpp
---

## API

- `polygon3_triangulation_indices(polygon)`: 多角形を支持平面の二次元座標へ写して三角形分割し、各三角形を元の `Polygon3` に対する 3 頂点の添字の配列として返す。三角形は二次元座標上で反時計回りにそろえる。頂点数を $N$ とすると、結果は最大 $N-2$ 個である（連続する重複点や辺上の中間点を除いた後の頂点数 $N'$ に対して $N'-2$ 個）。

## API別の時間計算量・空間計算量

- `polygon3_triangulation_indices`: 単調分割を用いるため時間 $O(N\log N)$、返り値を含む領域 $O(N)$。

## 注意点

頂点を境界の順に並べた、自己交差がなく面積が正の共面多角形を与える。共面性は検査せず、支持平面へ射影して扱う。
頂点数が 3 未満、面積が 0（許容誤差 `GEOMETRY3D_EPS` 以下）、単純でないなどの不正な入力には `std::invalid_argument` を送出する。
