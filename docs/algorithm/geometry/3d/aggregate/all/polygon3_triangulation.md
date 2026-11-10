---
title: Polygon3 Triangulation Umbrella (三次元多角形の三角形分割集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/polygon3_triangulation.hpp
---

このヘッダは、2 つの戻り値の形式の個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。必要な戻り値の形式だけを使う場合は、`polygon3_triangulation_indices.hpp` または `polygon3_triangulation_triangles.hpp` を直接 include すればよい。

## API

- `polygon3_triangulation_indices(polygon)`: 三次元の単純多角形を三角形分割し、元の頂点の添字 3 つからなる配列の列 `std::vector<std::array<std::size_t, 3>>` で返す。
- `polygon3_triangulation(polygon)`: 同じ分割結果を `std::vector<Triangle3>` で返す。

いずれも `polygon3_to_2d` で多角形を平面座標に変換してから、二次元の単純多角形の三角形分割を行う。

## API 別の時間計算量・空間計算量

- 頂点数を $N$ として、各 API は時間 $O(N^2)$、戻り値を除く追加空間 $O(N)$。戻り値の列は $O(N)$。

## 注意点

頂点数が 3 未満なら `std::invalid_argument` を送出する。
