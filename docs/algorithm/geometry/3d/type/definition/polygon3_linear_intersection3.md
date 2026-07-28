---
title: Polygon3 Linear Intersection Result (三次元多角形と線形図形の交差結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/polygon3_linear_intersection3.hpp
---

## API

- `Polygon3LinearIntersection3::points`: どの返却線分にも含まれない孤立交点。
- `Polygon3LinearIntersection3::segments`: 共通部分に含まれる閉線分。
- `empty()`: `points` と `segments` がともに空なら `true` を返す。

## API別の時間計算量・空間計算量

- `empty()`: 時間・追加領域 $O(1)$。
- 構築・fieldへのアクセス: 返却vectorの要素数を除き時間・追加領域 $O(1)$。

## 注意点

型自体は点・線分の順序、重複、包含関係を検査しない。交差計算APIは孤立点と互いに内部が交わらない線分を格納する。
