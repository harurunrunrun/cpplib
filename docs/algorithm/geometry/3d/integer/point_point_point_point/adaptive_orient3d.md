---
title: Adaptive Orient3D (適応的精度による三次元向き判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/point_point_point_point/adaptive_orient3d.hpp
---

## API

```cpp
int adaptive_orient3d(first, second, third, fourth)
```

`orient3d` と同じ向きの正確な符号を `-1`, `0`, `1` で返す。まず正規化した `long double` の行列式と誤差上界を評価し、符号が確定できない場合だけ、展開（expansion）演算と、入力の `long double` を正確な 2 進有理数（dyadic）に変換したライブラリ内の `ExactInteger` による演算へ進む。最終的な符号は正確な演算の結果で決まる。

## API別の時間計算量・空間計算量

高速経路は時間・追加領域ともに $O(1)$。フォールバックは、入力の指数の幅と仮数のビット数に応じた多倍長整数演算の時間・領域を使う。点数・次元は固定である。

## 注意点

全入力は有限でなければならず、違反時は `std::invalid_argument` を送出する。返すのは入力として渡された `long double` 値に対する正確な向きであり、それ以前の 10 進文字列からの変換誤差は対象外である。退化した入力（4 点が共面）には 0 を返す。
座標の構成は行わず、判定と構成を分離している。
