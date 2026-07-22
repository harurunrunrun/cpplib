---
title: Count Primes (素数個数) [ALDS1_1_C]
documentation_of: ../src/algorithm/math/number_theory/count_primes.hpp
---

## API

- `math::count_primes(values)`: 整数range `values` に含まれる素数の個数を、
  各要素への決定的Miller–Rabin判定で求める。疎な大きい値に適する。
- `math::count_primes_with_sieve(values, maximum_value)`:
  `[0, maximum_value]` の素数表を一度構築して個数を求める。同じ小さい値域に
  要素が密集している場合に適する。

## API別の時間計算量・空間計算量

要素数を $N$、要素の絶対値の最大値を $U$、`maximum_value` を $M$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `count_primes` | $O(N\log(U+1))$ | $O(1)$ |
| `count_primes_with_sieve` | $O(M\log\log(M+2)+N)$ | $O(M)$ |

## 注意点

- 空rangeでは0を返す。符号付き整数の負数、0、1は素数として数えない。
- `count_primes` は各非負要素を64 bit符号なし整数へ変換する。
- 篩版へ渡す非負要素は `maximum_value` 以下でなければならず、違反時は
  `std::invalid_argument` を送出する。表の長さを表現できない上限では
  `std::length_error` を送出する。
- 篩表の構築費があるため、少数の巨大値には `count_primes` を使う。
