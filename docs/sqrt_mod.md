---
title: Square Root Modulo a Prime (素数を法とする平方根)
documentation_of: ../src/algorithm/math/sqrt_mod.hpp
---

素数を法とする平方根をTonelli--Shanks法で求める。

## `sqrt_mod`

```cpp
optional<uint64_t> math::sqrt_mod(uint64_t value, uint64_t prime);
```

$x^2\equiv value\pmod {prime}$を満たす$x$が存在すれば、2つの平方根のうち
小さい方を返す。存在しなければ`nullopt`を返す。`value == 0`と`prime == 2`も
扱う。

`prime`は素数、`2 <= prime`、`value < prime`を要求する。関数内では
素数判定を行わない。範囲条件を満たさない場合は`invalid_argument`を送出する。

$prime-1=q2^s$、$q$は奇数とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| modular multiplication | $O(1)$ | $O(1)$ |
| modular power | $O(\log prime)$ | $O(1)$ |
| `sqrt_mod` | $O(s^2+Z\log prime)$ | $O(1)$ |

`Z`は2から最初の平方非剰余までに調べる候補数である。Library Checkerの
`prime <= 10^9`では決定的な昇順探索を行う。乗算には128bit整数を使用する。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
