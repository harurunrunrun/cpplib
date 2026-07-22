---
title: Adaptive In-Sphere (適応的精度による三次元外接球判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/adaptive_insphere.hpp
---

## API

```cpp
int adaptive_insphere(first, second, third, fourth, query)
```

四面体の頂点順に依存せず、外接球内なら1、球面上なら0、球外なら-1。正規化 `long double` determinantの誤差上界でfast pathを選び、不確かな場合は入力を正確なdyadic数へ変換したライブラリ内の `ExactInteger` determinantで符号を確定する。

## API別の時間計算量・空間計算量

fast pathは時間・追加領域 $O(1)$。fallbackは入力指数幅と仮数bit数に応じた多倍長整数演算時間・領域を使う。点数・次元は固定。

## 注意点

全入力は有限でなければならず、非有限入力および退化四面体では `std::invalid_argument`。返すのは述語の符号だけで、外接球の中心・半径は構成しない。これによりDelaunayの位相判定を構成計算の丸めから分離できる。
