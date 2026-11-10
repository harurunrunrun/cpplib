---
title: Large Factorial Table (大きな素数を法とする階乗表) [factorial] [many_factorials]
documentation_of: ../../../../src/algorithm/math/combinatorics/large_factorial_table.hpp
---

素数 $p$（テンプレート引数 `MOD`）を法とする階乗 $n! \bmod p$ を、$p$ が $10^9$ 程度と大きくても高速に答えるための表である。ブロック幅 `BLOCK` ごとのブロック積を多項式とみなして等差数列上の値へ外挿（Lagrange 補間を畳み込みで一括評価）し、ブロック境界の階乗を保存する。

## API

```cpp
template<int MOD, int BLOCK = 512>
class LargeFactorialTable;

explicit LargeFactorialTable(std::uint32_t maximum = MOD - 1);
Modint<MOD> factorial(std::uint64_t n) const;
Modint<MOD> range_product(std::uint64_t left, std::uint64_t right) const;
```

- コンストラクタ: $\min(\mathrm{maximum}, p - 1)$ までの階乗に答えられる表を構築する。
- `factorial(n)`: $n! \bmod p$ を返す。$n \ge p$ なら $0$ を返す。
- `range_product(left, right)`: 両端を含む整数区間 $[\mathrm{left}, \mathrm{right}]$ の積を返す。空区間（`left > right`）なら $1$、区間が $p$ の倍数（$0$ を含む）を含むなら $0$ を返す。区間端点は `uint64_t` で扱う。

## 時間計算量

ブロック数を $K = \lfloor \mathrm{maximum} / \mathrm{BLOCK} \rfloor$ とする。

- 構築: $O(\mathrm{BLOCK}^2 + K \log (K+1))$ 時間、$O(K + \mathrm{BLOCK})$ 空間
- `factorial`: $O(\mathrm{BLOCK})$
- `range_product`: $O(\mathrm{BLOCK} + \log p)$

## 注意点

- `MOD` は素数でなければならない。
- 必要な NTT 長が `MOD - 1` を割り切らない場合は 3 素数 CRT による任意法畳み込みを使うため、その変換長と復元範囲の制約も受ける。
- 必要な剰余（`factorial(n)` の $n$、`range_product` では端点を $p$ で割った余り）が構築時の上限を超える場合は `out_of_range` を送出する。
