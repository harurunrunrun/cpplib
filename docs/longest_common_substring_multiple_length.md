---
title: Longest Common Substring Multiple Length (複数文字列の最長共通部分文字列長) [LCS2]
documentation_of: ../src/algorithm/string/longest_common_substring_multiple_length.hpp
---

複数の文字列すべてに共通する連続部分文字列の最大長を求める。

## longest_common_substring_multiple_length

```cpp
template<int MAX_LENGTH, int ALPHABET = 26, char OFFSET = 'a'>
int longest_common_substring_multiple_length(
    span<const string> texts
);
```

`texts` の全要素に現れる連続部分文字列の最大長を返す。空列では0、文字列が1個ならその長さを返す。

## 時間計算量

最短文字列長を $L$、文字列数を $K$、全文字数を $S$ として
$O(ALPHABET \cdot MAX\_LENGTH + S + KL)$。

## 空間計算量

$O(ALPHABET \cdot MAX\_LENGTH + L + K)$。固定容量のSuffix Automaton本体は
heap上に確保する。

## 注意点

- 最短文字列長は `MAX_LENGTH` 以下でなければならない。
- 各文字 `c` は `OFFSET <= c < OFFSET + ALPHABET` を満たさなければならない。
- 前提違反時は `runtime_error` を送出する。
- 戻り値は長さのみで、位置や内容は返さない。
