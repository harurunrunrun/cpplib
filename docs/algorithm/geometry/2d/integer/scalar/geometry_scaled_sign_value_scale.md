---
title: Geometry Scaled Sign with One Scale (単一scaleによる幾何符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/scalar/geometry_scaled_sign_value_scale.hpp
---

## API

- `geometry_scaled_sign(value, scale)`: 同じ `scale` を幾何許容誤差と丸め誤差の両方へ使い、`value` の符号を `-1,0,1` で返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

3引数coreへ `geometry_scaled_sign(value, scale, scale)` として委譲する。scaleの符号は結果に影響せず、許容幅との境界上では0を返す。
