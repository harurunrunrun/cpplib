---
title: Palindromic Factorization (回文分解)
documentation_of: ../../../../src/algorithm/string/palindrome/palindromic_factorization.hpp
---

文字列を連続する回文に分割するときの最小の因子数と、その分割の一例を求める。
回文木の series link 上で動的計画法を行う。

## API

```cpp
struct PalindromicFactorizationResult {
    int factor_count = 0;
    std::vector<std::pair<int, int>> factors;
};

PalindromicFactorizationResult palindromic_factorization(
    const std::string& text
)
```

`factor_count` は最小の因子数、`factors` は各因子を元の文字列上の半開区間 `[l, r)` として左から順に並べたものである。
空文字列では因子数 0、区間の列は空である。

## 時間計算量・空間計算量

$N=\lvert \mathrm{text}\rvert$ とする。回文木の各ノードは長さ 256 の遷移表を持つ。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `palindromic_factorization` | $O(N\log N+256N)$ | $O(256N)$ |

## 注意点

最適な分割が複数ある場合は、そのうちの 1 つを返す。
