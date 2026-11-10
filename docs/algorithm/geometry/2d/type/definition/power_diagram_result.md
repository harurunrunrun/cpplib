---
title: Power Diagram Result (パワー図結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/power_diagram_result.hpp
---

site ごとの有界なパワーセルを点列として格納する。

## API

```cpp
struct PowerDiagramResult {
    std::vector<std::vector<Point>> cells;
};
```

`cells[i]` は site `i` のセルを表す反時計回りの凸多角形で、空のセルは空の列になる。

## 時間計算量

メンバの参照は $O(1)$、全走査は全セルの頂点数の合計に対して線形。

## 注意点

各セルは、構築時に指定された外枠の多角形で切り取られている。
