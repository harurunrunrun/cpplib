---
title: Large Factorial Table (大きな素数を法とする階乗表) [factorial] [many_factorials]
documentation_of: ../../../../src/algorithm/math/combinatorics/large_factorial_table.hpp
---

`LargeFactorialTable<MOD, BLOCK>(maximum)` を構築し、`factorial(n)` で $n!$ を返す。`MOD` は素数、既定のブロック幅は512。ブロック積の多項式を等差数列上へ外挿し、ブロック境界の階乗を保存する。`range_product(left,right)` は両端を含む整数区間の積。空区間は1、法の倍数を含む区間は0。区間端点は `uint64_t` で扱う。

## 時間計算量

ブロック数を $K=\lfloor\mathrm{maximum}/\mathrm{BLOCK}\rfloor$ として、構築 $O(\mathrm{BLOCK}^2+K\log(K+1))$、空間 $O(K+\mathrm{BLOCK})$。`factorial` は $O(\mathrm{BLOCK})$、区間積は $O(\mathrm{BLOCK}+\log\mathrm{MOD})$。

## 注意点

`MOD` は素数であること。法に適したNTTがなければ3素数CRTを使うため、変換長と整数畳み込みの復元範囲の制約も受ける。`n>=MOD` の階乗は0。その他の照会に必要な剰余が構築上限を超えると `out_of_range`。
