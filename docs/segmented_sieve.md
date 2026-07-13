---
title: Segmented Sieve
documentation_of: ../src/algorithm/math/segmented_sieve.hpp
---

区間 `[l, r)` の素数判定をまとめて行う。

`0 <= l <= r` を仮定する。

## 関数

- `math::segmented_sieve(l, r)`
    - `l + i` が素数なら `true` となる列を返す。
- `math::primes_in_range(l, r)`
    - 区間 `[l, r)` に含まれる素数を昇順で返す。

## 計算量

- $O((r-l)\log\log r + \sqrt r \log\log r)$

## API別計算量

$L=r-l$、区間内の素数個数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `math::segmented_sieve_isqrt(n)` | $O(1)$ | $O(1)$ |
| `math::segmented_sieve(l,r)` | $O(\sqrt r\log\log r+L\log\log r)$ | $O(\sqrt r+L)$ |
| `math::primes_in_range(l,r)` | $O(\sqrt r\log\log r+L\log\log r+K)$ | $O(\sqrt r+L+K)$ |

`primes_in_range` は全 $L$ 個のフラグを走査し、さらに $K$ 個を出力する。
