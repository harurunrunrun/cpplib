---
title: Sampled Primes (一定間隔の素数列) [TDPRIMES]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/sampled_primes.hpp
---

上限未満の素数の列から、素数の番号が一定の合同類にあるものだけを返す。

## `sampled_primes`

```cpp
std::vector<int> math::sampled_primes(int exclusive_limit, int stride = 100);
```

`exclusive_limit` 未満の素数のうち、1 始まりの番号を `stride` で割った余りが $1$ であるもの（$1, \mathrm{stride}+1, 2\,\mathrm{stride}+1, \ldots$ 番目）を昇順に返す。

## 時間計算量・空間計算量

$L$ を `exclusive_limit` とする。奇数のみのエラトステネスの篩を用いる。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sampled_primes` | $O(L \log \log L)$ | $O(L)$ bit（出力を除く） |

## 注意点

- `exclusive_limit` は非負、`stride` は正でなければならず、違反時は `std::invalid_argument` を送出する。
