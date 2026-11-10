---
title: Enumerate Quotient Values (商の値列挙) [dirichlet_convolution_and_prefix_sums] [dirichlet_inverse_and_prefix_sums] [enumerate_quotients]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/enumerate_quotient_values.hpp
---

$1\le i\le N$ に対する $\lfloor N/i\rfloor$ の相異なる値を列挙する。

## math::enumerate_quotient_values

```cpp
vector<long long> math::enumerate_quotient_values(long long number)
```

`number` を $N$ として、相異なる $\lfloor N/i\rfloor$ を昇順で返す。`number == 0` では空列を返す。内部で `quotient_ranges` を使う。

## 時間計算量

$O(\sqrt{N})$。

## 空間計算量

戻り値と一時的な商の区間列を含めて $O(\sqrt{N})$。

## 注意点

- `number` は非負でなければならない。負数では `runtime_error` を送出する。
