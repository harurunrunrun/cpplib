---
title: GCD of Gaussian Integers
documentation_of: ../src/algorithm/math/gaussian_integer.hpp
---

Gauss整数$\mathbb Z[i]$上のEuclid法を提供する。

## `GaussianInteger`

```cpp
math::GaussianInteger{real, imag}
```

`real + imag * i`を表す。両成分は`long long`である。等値比較を使用できる。

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| 構築、`real` / `imag`参照、等値比較 | $O(1)$ | $O(1)$ |

## `gaussian_gcd`

```cpp
math::GaussianInteger math::gaussian_gcd(
    math::GaussianInteger left,
    math::GaussianInteger right
);
```

`left`と`right`のGauss整数上の最大公約数を返す。複素除算の実部・虚部を
それぞれ最も近い整数へ丸めて余りを作る。この余りのノルムは除数のノルムより
小さい。

単数$\{1,-1,i,-i\}$倍の不定性を除くため、非零の戻り値は
`real > 0 && imag >= 0`となるassociateへ正規化する。
`gcd(0, 0)`は`{0, 0}`である。途中成分が`long long`に収まらない場合は
`overflow_error`を送出する。

$N=|left|^2+|right|^2$とする。

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| Euclid法の1反復 | $O(1)$ | $O(1)$ |
| `gaussian_gcd` | $O(\log(N+1))$ | $O(1)$ |

積、ノルム、商の丸めには符号付き128bit整数を使用する。
