---
title: Longest Common Substring
documentation_of: ../src/algorithm/string/longest_common_substring.hpp
---

2 つの文字列の最長共通部分文字列を求める。

## 構造体

- `LongestCommonSubstringResult`
    - `first_pos`
    - `second_pos`
    - `length`

## 関数

- `longest_common_substring(a, b)`

## 計算量

- $O((n + m) \log(n + m))$

## API別計算量

$S=|a|+|b|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `longest_common_substring(a,b)` | $O(S\log S)$ | $O(S)$ |
| `LongestCommonSubstringResult` の各field参照 | $O(1)$ | - |

suffix array構築が支配的で、LCP構築と隣接suffix走査は各 $O(S)$。戻り値は位置と長さだけなので $O(1)$。
