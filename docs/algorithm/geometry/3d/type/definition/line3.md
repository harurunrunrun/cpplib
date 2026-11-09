---
title: 3D Line Type (三次元直線型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/line3.hpp
---

## API

- `Line3{a, b}`: 相異なる 2 点 `a`, `b` を通る直線を表す集成体型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

- 型自体は値を検査しない。`a == b` の退化した直線を許容するかどうかは、利用する個々の API に従う。
