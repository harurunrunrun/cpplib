---
title: Count Primes (素数個数) [ALDS1_1_C]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/count_primes.hpp
---

## API

- `math::count_primes(values)`: 整数を要素とする range `values` に含まれる素数の個数を、各要素への決定的な Miller–Rabin 判定で求める。値が大きく疎な場合に適する。
- `math::count_primes_with_sieve(values, maximum_value)`: $0$ 以上 `maximum_value` 以下の素数表をエラトステネスの篩で一度構築して個数を求める。小さい値域に要素が密集している場合に適する。

どちらも戻り値は `std::size_t`。

## 時間計算量・空間計算量

要素数を $N$、要素の絶対値の最大値を $U$、`maximum_value` を $M$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `count_primes` | $O(N \log(U+1))$ | $O(1)$ |
| `count_primes_with_sieve` | $O(M \log \log(M+2) + N)$ | $O(M)$ |

## 注意点

- 空の range では $0$ を返す。符号付き整数の負の値、$0$、$1$ は素数として数えない。
- `count_primes` は非負の各要素を 64 bit 符号なし整数に変換して判定する。
- 篩版に渡す非負の要素は `maximum_value` 以下でなければならず、違反時は `std::invalid_argument` を送出する。表の長さを表現できない上限（`std::size_t` の最大値）では `std::length_error` を送出する。
- 篩の表を構築するコストがかかるので、少数の巨大な値には `count_primes` を使う。
