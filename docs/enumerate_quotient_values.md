---
title: Enumerate Quotient Values (商の値列挙) [enumerate_quotients]
documentation_of: ../src/algorithm/math/number_theory/enumerate_quotient_values.hpp
---

$1\le i\le N$ に対する $\lfloor N/i\rfloor$ の相異なる値を列挙する。

## math::enumerate_quotient_values

```cpp
vector<long long> math::enumerate_quotient_values(long long number)
```

相異なる $\lfloor number/i\rfloor$ を昇順で返す。
`number == 0` では空列を返す。

## 時間計算量

$O(\sqrt{number})$。

## 空間計算量

戻り値と一時的な商区間列を含めて $O(\sqrt{number})$。

## 注意点

`number` は非負でなければならない。負数では `runtime_error` を送出する。
