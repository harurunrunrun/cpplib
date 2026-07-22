---
title: Normalize Convex Polygon (凸多角形の正規化)
documentation_of: ../src/algorithm/geometry/2d/point_collection/normalize_convex_polygon.hpp
---

## API

- `normalize_convex_polygon(polygon)`: 弱凸な周回列を、反時計回り・辞書順最小頂点始まりに正規化する。

連続重複、閉路末尾の先頭頂点、中間共線点を除き、全点共線なら両端だけを返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間・追加領域 $O(N)$。

## 注意点

入力は時計回りまたは反時計回りに境界を一周する弱凸な周回列でなければならない。
左右の折れ曲がりが混在する列に加え、辺方向が一周を超えて回転する自己交差星形や
同じ輪郭を複数周する列にも `std::invalid_argument` を送出する。
検証を含めて時間 $O(N)$ である。
