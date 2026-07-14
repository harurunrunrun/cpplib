---
title: Enumerate Palindrome Lengths (回文長列挙) [enumerate_palindromes]
documentation_of: ../src/algorithm/string/enumerate_palindrome_lengths.hpp
---

文字位置と文字間の各中心について、その中心を持つ最長回文部分文字列の長さを返す。

## enumerate_palindrome_lengths

```cpp
vector<int> enumerate_palindrome_lengths(const string& text)
```

長さ `N` の文字列に対して長さ `2N-1` の列を返す。
`result[2*i]` は文字 `i` を中心とする最長回文の長さ、
`result[2*i+1]` は文字 `i` と `i+1` の間を中心とする最長回文の長さである。
空文字列には空列を返す。

## 時間計算量

`N = text.size()` として $O(N)$。

## 空間計算量

Manacherの半径列と戻り値を含めて $O(N)$。

## 注意点

- 添字は0-indexed。
- 奇数位置の戻り値は偶数、偶数位置の戻り値は奇数になる。
