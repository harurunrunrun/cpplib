---
title: Segmented Sieve (区間篩)
documentation_of: ../src/algorithm/math/segmented_sieve.hpp
---

区間 `[l, r)` の素数判定をまとめて行う。

`0 <= l <= r` を仮定する。

## 関数

- `math::segmented_sieve(l, r)`
    - `l + i` が素数なら `true` となる列を返す。
- `math::primes_in_range(l, r)`
    - 区間 `[l, r)` に含まれる素数を昇順で返す。

## 計算量の概要

- $\sqrt r\le5{,}000{,}000$ では区間篩を使い、
  $O((r-l)\log\log r + \sqrt r\log\log r)$。
- それより大きい場合は各整数を決定的Miller--Rabinで判定し、
  $O((r-l)\log r)$。

## API別の時間計算量・空間計算量

$L=r-l$、区間内の素数個数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `math::segmented_sieve_isqrt(n)` | $O(1)$ | $O(1)$ |
| `math::segmented_sieve(l,r)` | $O(\sqrt r\log\log r+L\log\log r)$ または $O(L\log r)$ | $O(\sqrt r+L)$ または $O(L)$ |
| `math::primes_in_range(l,r)` | 上記に $O(L+K)$ を加えた時間 | 上記に $O(K)$ を加えた領域 |

`primes_in_range` は全 $L$ 個のフラグを走査し、さらに $K$ 個を出力する。

## 注意点

- `segmented_sieve_isqrt(n)` は内部でも使う整数平方根を返す低水準API。
  $n\ge0$ を前提とし、負数は検査しない。
- `segmented_sieve(l,r)` は長さ $r-l$ のflag列を返し、
  index $i$ は整数 $l+i$ が素数かを表す。
- `primes_in_range(l,r)` は同じ範囲の素数を昇順で返す。
- 公開範囲APIは $0\le l\le r$ を要求し、違反時は例外。
  空区間では空vectorを返す。
- $r$ が大きい狭い区間でも $O(\sqrt r)$ のbase sieveを確保せず、
  保持領域を区間長に比例させる。
