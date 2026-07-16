---
title: Deque Palindrome Queries (両端回文更新query) [palindromes_in_deque]
documentation_of: ../src/algorithm/string/deque_palindrome_queries.hpp
---

両端回文木の更新と、更新直後の回文統計取得を1回のmethodで行う。

## 構築

```cpp
DequePalindromeQueries<ALPHABET, MAX_NODES, MAX_LENGTH, OFFSET> queries
```

空文字列を作る。容量と文字範囲は `DequePalindromicTree` と同じ。

- 時間計算量: $O(MAX\_NODES\cdot ALPHABET+MAX\_LENGTH)$
- 保存領域: $O(MAX\_NODES\cdot ALPHABET+MAX\_LENGTH)$

## 更新と結果

```cpp
DequePalindromeStatistics push_front(char character)
DequePalindromeStatistics push_back(char character)
DequePalindromeStatistics pop_front()
DequePalindromeStatistics pop_back()
```

更新後の異なる非空回文数、最長回文prefix長、最長回文suffix長を返す。

- `push_front`, `push_back`: 最悪 $O(\log(N+1)+ALPHABET)$
- `pop_front`, `pop_back`: 最悪 $O(1)$
- 戻り値構築の追加時間・空間: $O(1)$

## その他

```cpp
int size() const
bool empty() const
```

いずれも $O(1)$。

## 注意点

- 空文字列に対する `pop_front` と `pop_back` は `runtime_error`。
- alphabet外文字、`MAX_LENGTH` 超過、`MAX_NODES` 超過でも `runtime_error`。
- pop後にも返り値は有効で、空文字列では3つの統計値がすべて0になる。
