---
title: Line Arrangement Result (直線アレンジメント結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/line_arrangement_result.hpp
---

直線アレンジメントの有限頂点、有界辺、ray、交点のない無限直線を格納する。

## API

- `LineArrangementEdge` は両端頂点と直線番号を持つ。
- `LineArrangementRay` は始点頂点、単位方向、直線番号を持つ。
- `LineArrangementUnboundedLine` は原点、単位方向、直線番号を持つ。
- `LineArrangementResult` は `vertices`, `bounded_edges`, `rays`, `unbounded_lines` を持つ。

## 時間計算量

member参照は $O(1)$、全走査は格納要素数に対して線形。

## 注意点

rayの方向は有限頂点から無限遠を向く。
