---
title: Point Set Maximum Area Triangle Result (点集合内最大面積三角形の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/point_set_maximum_area_triangle_result.hpp
---

## API

- `vertices`: 選ばれた3頂点。
- `doubled_area`: 三角形面積の2倍。
- `valid`: 3点以上から非退化三角形を選べたかを表す。
- `area()`: 面積を返す。

## API別の時間計算量・空間計算量

- 各member参照と `area()` は時間計算量・追加空間計算量 $O(1)$。

## 注意点

`valid == false` のとき `vertices` と面積値は有効な三角形を表さない。
