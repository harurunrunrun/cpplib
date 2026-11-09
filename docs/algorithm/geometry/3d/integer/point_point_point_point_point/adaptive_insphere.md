---
title: Adaptive In-Sphere (適応的精度による三次元外接球判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/point_point_point_point_point/adaptive_insphere.hpp
---

## API

```cpp
int adaptive_insphere(first, second, third, fourth, query)
```

四面体 `first`, `second`, `third`, `fourth` の外接球について、`query` が球の内部なら `1`、球面上なら `0`、外部なら `-1` を返す。結果は四面体の頂点順に依存しない。
正規化した `long double` の行列式と誤差上界で高速経路を選び、符号が確定できない場合は、入力を正確な 2 進有理数（dyadic）に変換したライブラリ内の `ExactInteger` による行列式で符号を確定する。

## API別の時間計算量・空間計算量

高速経路は時間・追加領域ともに $O(1)$。フォールバックは、入力の指数の幅と仮数のビット数に応じた多倍長整数演算の時間・領域を使う。点数・次元は固定である。

## 注意点

全入力は有限でなければならない。非有限な入力、および退化した四面体（`adaptive_orient3d` が 0）には `std::invalid_argument` を送出する。
返すのは述語の符号だけで、外接球の中心・半径は構成しない。これにより、Delaunay 分割の位相の判定を構成計算の丸めから切り離せる。
