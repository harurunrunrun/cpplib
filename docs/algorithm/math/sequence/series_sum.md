---
title: Series Sum (数列和)
documentation_of: ../../../../src/algorithm/math/sequence/series_sum.hpp
---

等差数列・等比数列の和を求める。除算を使わないため、`T` は整数からの構築と加算・乗算をもつ環であればよい。

## 関数

- `math::arithmetic_progression_sum(first, diff, n)`
    - 初項 `first`、公差 `diff`、項数 `n` の等差数列の和を返す。
- `math::geometric_series_pow_sum(ratio, n)`
    - $\mathrm{ratio}^n$ と $1 + \mathrm{ratio} + \cdots + \mathrm{ratio}^{n-1}$ の組 `{ratio^n, sum}` を返す。
- `math::geometric_series_sum(ratio, n)`
    - $1 + \mathrm{ratio} + \cdots + \mathrm{ratio}^{n-1}$ を返す。
- `math::geometric_progression_sum(first, ratio, n)`
    - 初項 `first`、公比 `ratio`、項数 `n` の等比数列の和を返す。

`n` は `long long` である。

## API別の時間計算量・空間計算量

`T` の演算を $O(1)$ とする。

| API | 時間計算量 |
| --- | --- |
| `math::arithmetic_progression_sum(first, diff, n)` | $O(1)$ |
| `math::geometric_series_pow_sum(ratio, n)` | $O(\log(n+1))$ |
| `math::geometric_series_sum(ratio, n)` | $O(\log(n+1))$ |
| `math::geometric_progression_sum(first, ratio, n)` | $O(\log(n+1))$ |

`geometric_series_pow_sum` は再帰で実装されており、再帰スタックは $O(\log(n+1))$ である。

## 注意点

- `n < 0` の場合は `std::runtime_error` を送出する。
- 整数型を `T` に使う場合、中間値のオーバーフローは補償しない。
