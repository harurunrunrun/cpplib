---
title: Adaptive Orient3D (適応的精度による三次元向き判定)
documentation_of: ../src/algorithm/geometry/3d/adaptive_orient3d.hpp
---

## API

```cpp
int adaptive_orient3d(first, second, third, fourth)
```

`orient3d` と同じ向きの正確な符号を `-1, 0, 1` で返す。まず正規化した `long double` determinantと誤差上界を評価し、符号が不確かな場合だけexpansion arithmeticと、入力 `long double` を正確なdyadic数へ変換したライブラリ内の `ExactInteger` 演算へ進む。

## API別の時間計算量・空間計算量

fast pathは時間・追加領域 $O(1)$。fallbackは入力指数幅と仮数bit数に応じた多倍長整数演算時間・領域を使う。点数・次元は固定。

## 注意点

全入力は有限でなければならず、違反時は `std::invalid_argument`。返すのは入力として渡された `long double` 値の正確な向きであり、それ以前の10進文字列からの変換誤差は対象外。退化入力は0。座標構成を行わず、判定と構成を分離している。
