---
title: GCD of Gaussian Integers (ガウス整数の最大公約数) [gcd_of_gaussian_integers]
documentation_of: ../../../../../src/algorithm/math/number_theory/gaussian/gaussian_integer.hpp
---

ガウス整数環 $\mathbb Z[i]$ 上のユークリッドの互除法を提供する。

## `GaussianInteger`

```cpp
math::GaussianInteger{real, imag}
```

$\mathrm{real} + \mathrm{imag} \cdot i$ を表す。両成分は `long long` で、等値比較を使用できる。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 構築、`real` / `imag` の参照、等値比較 | $O(1)$ | $O(1)$ |

## `gaussian_gcd`

```cpp
math::GaussianInteger math::gaussian_gcd(
    math::GaussianInteger left,
    math::GaussianInteger right
);
```

`left` と `right` のガウス整数としての最大公約数を返す。複素数としての商の実部・虚部をそれぞれ最も近い整数へ丸めて余りを作るので、余りのノルムは除数のノルムより真に小さくなる。

単数 $\lbrace 1, -1, i, -i \rbrace$ 倍の不定性を除くため、非零の戻り値は `real > 0 && imag >= 0` を満たす同伴元へ正規化する。`gaussian_gcd({0, 0}, {0, 0})` は `{0, 0}` を返す。

$N = \lvert \mathrm{left} \rvert^2 + \lvert \mathrm{right} \rvert^2$ とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 互除法の 1 反復 | $O(1)$ | $O(1)$ |
| `gaussian_gcd` | $O(\log(N+1))$ | $O(1)$ |

## 注意点

- 積・ノルム・商の丸めは符号付き 128 bit 整数（`__int128`）で計算する。
- 互除法の途中の余りや正規化後の成分が `long long` に収まらない場合は `std::overflow_error` を送出する。
