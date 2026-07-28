---
title: Garner's Algorithm (Garner法)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/garner_algorithm.hpp
---

互いに素な法に対する連立合同式の解を、法の積を構築せず混合基数表示へ変換して任意の法で求める。

## API

```cpp
math::u64 math::garner_algorithm(
    const vector<math::u64>& remainders,
    const vector<math::u64>& moduli,
    math::u64 output_modulus
);
```

連立合同式を満たす最小非負整数を `output_modulus` で割った余りを返す。

## 時間計算量

合同式数を $N$ として $O(N^2\log M)$ 時間、$O(N)$ 空間。互いに素かの検査も $O(N^2\log M)$。

## 注意点

入力法はすべて `2` 以上かつ相互に素、出力法は正でなければならない。違反時は `runtime_error` を送出する。
