---
title: Segmented Sieve (区間篩)
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/segmented_sieve.hpp
---

半開区間 $[l, r)$ の整数の素数判定をまとめて行う。

## 関数

- `math::segmented_sieve(l, r)`
    - 長さ $r - l$ の `std::vector<char>` を返す。添字 $i$ の要素は、整数 $l + i$ が素数なら `true` である。
- `math::primes_in_range(l, r)`
    - 区間 $[l, r)$ に含まれる素数を昇順に返す。
- `math::segmented_sieve_isqrt(n)`
    - 内部でも使う整数平方根 $\lfloor \sqrt n \rfloor$ を返す低水準の API。

引数はすべて `long long`。

## アルゴリズム

- $\lfloor \sqrt{r-1} \rfloor + 1 \le 5 \times 10^6$ のときは、$\sqrt r$ 程度までの素数表を作って区間篩を行う。時間 $O((r-l) \log \log r + \sqrt r \log \log r)$。
- それより大きい場合は、区間内の各整数を決定的な Miller–Rabin 判定で調べる。時間 $O((r-l) \log r)$。$r$ が大きく区間が狭い場合でも $O(\sqrt r)$ の表を確保せず、保持領域は区間長に比例する。

## 時間計算量・空間計算量

$L = r - l$、区間内の素数の個数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `math::segmented_sieve_isqrt(n)` | $O(1)$ | $O(1)$ |
| `math::segmented_sieve(l, r)` | $O(\sqrt r \log \log r + L \log \log r)$ または $O(L \log r)$ | $O(\sqrt r + L)$ または $O(L)$ |
| `math::primes_in_range(l, r)` | 上記に $O(L + K)$ を加えた時間 | 上記に $O(K)$ を加えた領域 |

`primes_in_range` は $L$ 個のフラグをすべて走査し、$K$ 個を出力する。

## 注意点

- `segmented_sieve` と `primes_in_range` は $0 \le l \le r$ を要求し、違反時は `std::runtime_error` を送出する。空区間では空の列を返す。
- `segmented_sieve_isqrt(n)` は $n \ge 0$ を前提とし、負の値は検査しない。
