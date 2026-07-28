---
title: Constrained Delaunay Triangulation Result (制約付きDelaunay三角形分割結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/constrained_delaunay_triangulation_result.hpp
---

制約付きDelaunay三角形分割の三角形、全辺、制約辺を格納する。

## API

`triangles`, `edges`, `constraints` の各公開memberを持つ。

## 時間計算量

member参照は $O(1)$、各列の全走査は列長に対して線形。

## 注意点

頂点番号は元の点列の0-based index。
