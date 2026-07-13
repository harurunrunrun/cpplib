---
title: Minimum of a Linear Function Modulo m (一次式の剰余の最小値)
documentation_of: ../src/algorithm/math/min_of_mod_of_linear.hpp
---

等差数列を法で折り返した列の最小値を求める。

## `min_of_mod_of_linear`

```cpp
long long math::min_of_mod_of_linear(
    long long count,
    long long modulus,
    long long coefficient,
    long long constant
);
```

次の値を返す。

$$
\min_{0\le x<count}(coefficient\cdot x+constant)\bmod modulus
$$

`count > 0`, `modulus > 0`,
`0 <= coefficient, constant < modulus`を要求する。条件を満たさない場合は
`invalid_argument`を送出する。

係数が法の半分より大きいときは添字を反転し、係数を`modulus - coefficient`へ
置き換える。その後、法の倍数を初めて越えた位置だけを次の問題へ写す。
次の問題の法は現在の係数以下であり、係数反転により現在の法の半分以下になる。

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `min_of_mod_of_linear` | $O(\log(modulus+1))$ | $O(1)$ |

積と加算の途中計算には128bit整数を使用する。
