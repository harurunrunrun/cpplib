---
title: Minimum Enclosing Ellipse Result (最小包含楕円結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/minimum_enclosing_ellipse_result.hpp
---

楕円中心と対称二次形式を格納する。

## API

`evaluate(point)` は中心との差に二次形式を適用する。1以下なら楕円内を表す。

## 時間計算量

`evaluate` と全member参照は $O(1)$、空間 $O(1)$。

## 注意点

`approximation_tolerance` は構築時の停止許容誤差。
