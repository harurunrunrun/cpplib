---
title: Garner's Algorithm (Garner法)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/garner_algorithm.hpp
---

互いに素な法をもつ連立合同式の解を、法の積を直接構築せずに混合基数表示を経由して求め、任意の法での値を返す。

## API

```cpp
math::u64 math::garner_algorithm(
    const std::vector<math::u64>& remainders,
    const std::vector<math::u64>& moduli,
    math::u64 output_modulus
);
```

$x \equiv \mathrm{remainders}[i] \pmod{\mathrm{moduli}[i]}$ をすべて満たす最小の非負整数 $x$ を、`output_modulus` で割った余りを返す。剰余は内部で各法に対して正規化する。

## 時間計算量・空間計算量

合同式の個数を $N$、法の最大値を $M$ とする。混合基数表示の計算は時間 $O(N^2 + N \log M)$、互いに素かどうかの検査は $O(N^2 \log M)$ で、全体で時間 $O(N^2 \log M)$、空間 $O(N)$。

## 注意点

- 入力の法はすべて $2$ 以上で、どの 2 つも互いに素でなければならない。また `remainders` と `moduli` の長さは等しく、`output_modulus` は正でなければならない。違反時は `std::runtime_error` を送出する。
- 法と `output_modulus` は `u64` の範囲（$2^{64}$ 未満）であれば $2^{63}$ 以上でもよい。内部の加算はオーバーフローしないように行う。
