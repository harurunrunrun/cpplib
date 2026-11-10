---
title: Enumerate Palindrome Lengths (回文長列挙) [enumerate_palindromes]
documentation_of: ../../../../src/algorithm/string/palindrome/enumerate_palindrome_lengths.hpp
---

文字の位置と文字の間の各中心について、その中心を持つ最長の回文部分文字列の長さを返す。

## enumerate_palindrome_lengths

```cpp
vector<int> enumerate_palindrome_lengths(const string& text)
```

長さ $N$ の文字列に対して、長さ $2N-1$ の列を返す。
`result[2 * i]` は文字 `i` を中心とする最長回文の長さ、
`result[2 * i + 1]` は文字 `i` と `i + 1` の間を中心とする最長回文の長さである。
空文字列には空の列を返す。

## 時間計算量

$N$ を `text.size()` として $O(N)$。

## 空間計算量

Manacher 法の半径の列と戻り値を含めて $O(N)$。

## 注意点

- 添字は 0-indexed である。
- 奇数番目の要素は偶数（0 もありうる）、偶数番目の要素は奇数になる。
