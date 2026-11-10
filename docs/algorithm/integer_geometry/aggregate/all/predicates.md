---
title: Exact Geometry Predicate Aggregates (厳密幾何判定集約)
documentation_of: ../../../../../src/algorithm/integer_geometry/aggregate/all/predicates.hpp
---

向き、包含、平行・直交、交差などの厳密な判定 API と、外積・内積を一括で読み込む。

## API

このヘッダ自体は関数を定義せず、次のヘッダを読み込む。

- `enumeration/segment_segment/segment_intersection_kind.hpp`
- `integer/point_point_point/orientation.hpp`
- `predicate/line_line/direction_relations.hpp`
- `predicate/line_point/on_line.hpp`
- `predicate/segment_point/on_segment.hpp`
- `predicate/segment_segment/direction_relations.hpp`
- `predicate/segment_segment/segments_intersect.hpp`
- `predicate/vector_vector/direction_relations.hpp`
- `scalar/point_point_point/cross_points.hpp`
- `scalar/vector_vector/vector_products.hpp`
- `aggregate/all/types.hpp（型の集約ヘッダ）`

## API別の時間計算量・空間計算量

- 集約ヘッダ自体: 実行時の処理はなく、追加領域は $O(1)$。

## 注意点

計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
