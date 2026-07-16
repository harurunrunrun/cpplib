---
title: 15-digit Integer Factorization (15桁整数の素因数分解) [FACT0]
documentation_of: ../src/algorithm/math/integer_factorization_15_digits.hpp
---

15桁未満の正整数を素因数と指数の昇順列へ分解する。

## `integer_factorization_15_digits`

```cpp
vector<pair<uint64_t,int>> integer_factorization_15_digits(uint64_t value);
```

各要素を `{prime, exponent}` として返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `integer_factorization_15_digits` | Pollard's rho の期待素因数分解時間 | $O(\log value)$ |

## 注意点

- `1 <= value < 10^15` が必要である。
