---
title: Palindromic Factorization (回文分解)
documentation_of: ../../../../src/algorithm/string/palindrome/palindromic_factorization.hpp
---

文字列を連続する回文へ分割するときの最小factor数と、その分割例を求める。
palindromic treeのseries link上で動的計画法を行う。

## API

```cpp
PalindromicFactorizationResult palindromic_factorization(
    const std::string& text
)
```

結果は `factor_count` と、元文字列上の半開区間 `factors` を持つ。空文字列では
factor数0、区間列は空である。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `palindromic_factorization` | $O(N\log N+256N)$ | $O(256N)$ |

## 注意点

最適分割が複数ある場合はそのうち1つを返す。
