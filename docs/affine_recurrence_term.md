---
title: Huge-exponent Affine Recurrence (巨大指数の一次漸化式) [REC]
documentation_of: ../src/algorithm/math/sequence/affine_recurrence_term.hpp
---

係数と添字を十進文字列で受け取り、一次漸化式の値を法付きで返す。

## `affine_recurrence_term`

```cpp
uint64_t affine_recurrence_term(
    const string& a,
    const string& b,
    const string& n,
    uint64_t modulus
);
```

`F_0=1, F_i=a*F_(i-1)+b` で定まる `F_n mod modulus` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `affine_recurrence_term` | $O(\lvert a\rvert+\lvert b\rvert+\lvert n\rvert)$ 回の法演算 | $O(1)$ |

## 注意点

- 文字列は符号なし十進表記、法は正でなければならない。
