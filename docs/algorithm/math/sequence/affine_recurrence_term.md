---
title: Huge-exponent Affine Recurrence (巨大指数の一次漸化式) [REC]
documentation_of: ../../../../src/algorithm/math/sequence/affine_recurrence_term.hpp
---

係数と添字を十進文字列で受け取り、一次漸化式の項の値を、指定した法で返す。

## `math::affine_recurrence_term`

```cpp
std::uint64_t math::affine_recurrence_term(
    const std::string& a,
    const std::string& b,
    const std::string& n,
    std::uint64_t modulus
);
```

$F_0 = 1$, $F_i = a F_{i-1} + b$ で定まる $F_n \bmod \mathrm{modulus}$ を返す。
$n$ の十進表記を上位桁から読み、一次関数の合成を $10$ 乗しながら桁の分だけ合成する。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `affine_recurrence_term` | $O(\lvert a\rvert+\lvert b\rvert+\lvert n\rvert)$ 回の法演算 | $O(1)$ |

ここで $\lvert a\rvert$ などは文字列の長さである。

## 注意点

- 文字列は空でない符号なし十進表記でなければならない。空文字列や数字以外の文字を含む場合は `std::invalid_argument` を送出する。
- `modulus` は正でなければならず、$0$ の場合は `std::invalid_argument` を送出する。
