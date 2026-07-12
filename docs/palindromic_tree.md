---
title: Palindromic Tree
documentation_of: ../src/algorithm/string/palindromic_tree.hpp
---

Palindromic Tree。

# テンプレート引数

```cpp
PalindromicTree<ALPHABET, MAX_NODES, OFFSET>
```

# construct

```cpp
PalindromicTree()
PalindromicTree(string_view s)
int add(char c)
```

# query

```cpp
int distinct_palindromes()
int longest_suffix_palindrome_length()
void build_occurrences()
int count_palindromic_substrings()
bool contains(string_view s)
int occurrence_count(string_view s)
```

`occurrence_count` は `build_occurrences` を内部で呼ぶ。

## 時間計算量

- 構築: $O(N)$
- occurrence 前計算: $O(V)$
- contains / occurrence_count: $O(VL)$
