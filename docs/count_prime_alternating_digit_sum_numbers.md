---
title: Prime Alternating Digit-sum Count (交互桁和が素数となる整数の個数) [LUCIFER]
documentation_of: ../src/algorithm/math/number_theory/count_prime_alternating_digit_sum_numbers.hpp
---

十進表記の偶数位置の桁和から奇数位置の桁和を引いた値が正の素数となる整数を、
符号なし整数の閉区間で数えます。

## API

### `count_prime_alternating_digit_sum_numbers(lower, upper)`

```cpp
namespace math {
uint64_t count_prime_alternating_digit_sum_numbers(
    uint64_t lower,
    uint64_t upper
);
}
```

位置は最下位桁を1番目として数えます。`lower <= value <= upper` を満たし、
`(偶数位置の桁和) - (奇数位置の桁和)` が正の素数となる `value` の個数を
返します。先頭の0は桁和へ影響しません。

## API別の時間計算量・空間計算量

`upper` の十進桁数を $D$ とします。 `uint64_t` では $D\leq 20$ です。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_prime_alternating_digit_sum_numbers` | $O(10D^2)$ | $O(D^2)$ |

## 注意点

- `lower <= upper` が必要です。満たさない場合は `invalid_argument` を
  送出します。
- `0` は条件を満たしません。
