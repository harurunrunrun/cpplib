---
title: Sampled Primes (一定間隔の素数列) [TDPRIMES]
documentation_of: ../src/algorithm/math/sampled_primes.hpp
---

上限未満の素数列から、素数添字が一定合同類にある要素を返す。

## `sampled_primes`

```cpp
vector<int> sampled_primes(int exclusive_limit, int stride = 100);
```

1始まりの素数添字が `1 mod stride` である素数を昇順に返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sampled_primes` | $O(exclusive\_limit\log\log exclusive\_limit)$ | $O(exclusive\_limit)$ bit |

## 注意点

- 上限は非負、`stride` は正でなければならない。
