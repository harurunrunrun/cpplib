---
title: Normalize Convex Polygon (凸多角形の正規化)
documentation_of: ../src/algorithm/geometry/advanced/normalize_convex_polygon.hpp
---

## API

- `normalize_convex_polygon(polygon)`: 弱凸な周回列を、反時計回り・辞書順最小頂点始まりに正規化する。

連続重複、閉路末尾の先頭頂点、中間共線点を除き、全点共線なら両端だけを返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間・追加領域 $O(N)$。

## 注意点

入力は時計回りまたは反時計回りの弱凸な周回列でなければならない。左右の折れ曲がりが混在すると `std::invalid_argument` を送出する。
