---
title: Prime Alternating Digit-sum Count (交互桁和が素数となる整数の個数) [LUCIFER]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/count_prime_alternating_digit_sum_numbers.hpp
---

10 進表記で、偶数番目の桁の和から奇数番目の桁の和を引いた値が正の素数になる整数を、符号なし整数の閉区間で数える。

## API

### `count_prime_alternating_digit_sum_numbers(lower, upper)`

```cpp
namespace math {
std::uint64_t count_prime_alternating_digit_sum_numbers(
    std::uint64_t lower,
    std::uint64_t upper
);
}
```

桁の位置は最下位桁を 1 番目として数える。$\mathrm{lower} \le v \le \mathrm{upper}$ を満たし、（偶数番目の桁の和）$-$（奇数番目の桁の和）が正の素数になる $v$ の個数を返す。先頭の 0 は桁和に影響しない。桁 DP で $[0, x]$ の個数を数え、差をとって求める。

## 時間計算量・空間計算量

`upper` の 10 進の桁数を $D$ とする。`uint64_t` では $D \le 20$ である。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_prime_alternating_digit_sum_numbers` | $O(10D^2)$ | $O(D^2)$ |

## 注意点

- `lower <= upper` が必要で、満たさない場合は `std::invalid_argument` を送出する。
- $0$ は条件を満たさない。
