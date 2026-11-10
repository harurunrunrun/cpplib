---
title: Longest Common Substring Multiple Length (複数文字列の最長共通部分文字列長) [LCS2]
documentation_of: ../../../../src/algorithm/string/suffix/longest_common_substring_multiple_length.hpp
---

複数の文字列すべてに共通する連続部分文字列の最大長を求める。

## API

```cpp
template<int MAX_LENGTH = -1, int ALPHABET = 26, char OFFSET = 'a'>
int longest_common_substring_multiple_length(
    std::span<const std::string> texts
);
```

`texts` のすべての要素に現れる連続部分文字列の最大長を返す。
`texts` が空なら `0`、文字列が 1 個ならその長さを返す。
最短の文字列から suffix automaton を構築し、残りの文字列で照合する。

`MAX_LENGTH` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` で状態を確保する。
`MAX_LENGTH` を明示すると、状態数 `2 * MAX_LENGTH + 1` の固定容量の suffix automaton をヒープ上に確保する。

```cpp
auto result = longest_common_substring_multiple_length(texts);
auto fixed = longest_common_substring_multiple_length<100000>(texts);
```

## 時間計算量

最短の文字列長を $L$、文字列数を $K$、全文字数を $S$、アルファベットの大きさ `ALPHABET` を $A$ とする。

- 可変長版: $O(AL + KL + S)$
- 固定容量版: $O(A \cdot \texttt{MAX_LENGTH} + KL + S)$

## 空間計算量

- 可変長版: $O(AL + K)$
- 固定容量版: $O(A \cdot \texttt{MAX_LENGTH} + L + K)$

## 注意点

- 固定容量版では、最短の文字列長が `MAX_LENGTH` 以下でなければならない。また、いずれの版でも最短の文字列長は `(INT_MAX - 1) / 2` 以下でなければならない。
- 各文字 `c` は `OFFSET <= c < OFFSET + ALPHABET` を満たさなければならない。
- 前提に違反した場合は `std::runtime_error` を送出する。
- 戻り値は長さのみで、位置や内容は返さない。
