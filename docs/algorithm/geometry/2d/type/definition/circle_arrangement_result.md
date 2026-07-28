---
title: Circle Arrangement Result (円アレンジメント結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/circle_arrangement_result.hpp
---

円アレンジメントの頂点と円弧を格納する結果型。

## API

`CircleArrangementArc` は円番号、両端頂点、角度範囲、全円フラグを持つ。
`CircleArrangementResult` は `vertices` と `arcs` を持つ。

## 時間計算量

各member参照は $O(1)$、全走査は格納要素数に対して線形。

## 注意点

頂点を持たない全円arcでは頂点番号は `size_t(-1)`。
