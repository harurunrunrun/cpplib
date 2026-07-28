---
title: Power Diagram Result (パワー図結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/power_diagram_result.hpp
---

siteごとの有界Power cellを点列として格納する。

## API

`cells[i]` はsite `i` の反時計回り凸多角形で、空cellは空列。

## 時間計算量

member参照は $O(1)$、全走査は全cell頂点数に対して線形。

## 注意点

cellは指定されたbounding polygonで切り取られている。
