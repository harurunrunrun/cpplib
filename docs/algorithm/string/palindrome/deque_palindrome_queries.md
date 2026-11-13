---
title: Deque Palindrome Queries (両端回文更新query) [palindromes_in_deque]
documentation_of: ../../../../src/algorithm/string/palindrome/deque_palindrome_queries.hpp
---

両端回文木（`DequePalindromicTree`）の更新と、更新直後の回文統計の取得を 1 回のメソッド呼び出しで行う。

## 構築

```cpp
DequePalindromeQueries<ALPHABET, MAX_NODES = -1, MAX_LENGTH = -1, OFFSET = 'a'> queries;
```

空文字列を作る。容量と文字範囲の意味は `DequePalindromicTree` と同じである。
`MAX_NODES` と `MAX_LENGTH` のどちらかが `-1`（省略時）なら、実行時サイズに合わせて `std::vector` を伸ばす可変長版になる。

```cpp
DequePalindromeQueries<26> dynamic;
DequePalindromeQueries<26, 100002, 100000> fixed;
```

- 時間計算量: 固定長版 $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET}+\texttt{MAX_LENGTH})$
- 空間計算量: 固定長版 $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET}+\texttt{MAX_LENGTH})$。可変長版は、作られたノード数 $S$、最大保持長 $L$、$A$ を `ALPHABET` として $O(AS+L)$

## 更新と結果

```cpp
DequePalindromeStatistics push_front(char character)
DequePalindromeStatistics push_back(char character)
DequePalindromeStatistics pop_front()
DequePalindromeStatistics pop_back()
```

更新後の、相異なる空でない回文の個数、最長の回文接頭辞の長さ、最長の回文接尾辞の長さを返す。

$N$ を更新直前の文字列長とする。

- `push_front`, `push_back`: 最悪 $O(\log(N+1)+\texttt{ALPHABET})$
- `pop_front`, `pop_back`: 最悪 $O(1)$
- 戻り値の構築にかかる追加の時間・空間: $O(1)$

## その他

```cpp
int size() const
bool empty() const
```

いずれも時間計算量は $O(1)$。

## 注意点

- 空文字列に対する `pop_front` と `pop_back` は `std::runtime_error` を送出する。
- 範囲外の文字、`MAX_LENGTH` の超過、`MAX_NODES` の超過でも `std::runtime_error` を送出する。
- pop 後の返り値も有効であり、空文字列では 3 つの統計値がすべて 0 になる。
