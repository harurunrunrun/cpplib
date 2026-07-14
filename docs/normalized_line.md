---
title: Normalized Line Equation (直線の正規化方程式)
documentation_of: ../src/algorithm/geometry/2d/normalized_line.hpp
---

## API

`NormalizedLine normalized_line(line)`

## 引数

非退化直線を渡す。

## 戻り値

同じ直線を表す `a*x + b*y + c = 0` の係数を返す。`hypot(a,b)=1` で、`a>0` または `a==0 && b>=0` に正規化する。

## API別の時間計算量・空間計算量

- `normalized_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線では `invalid_argument`。浮動小数点演算の丸めを含む。
