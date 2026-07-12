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
int longest_palindrome_length()
int longest_palindrome_node()
void build_occurrences()
int count_palindromic_substrings()
int find_node(string_view s)
bool contains(string_view s)
int occurrence_count(string_view s)
string palindrome(int v)
pair<int,int> first_occurrence_range(int v)
vector<int> palindrome_nodes()
vector<string> palindromes()
vector<int> suffix_palindrome_nodes()
vector<int> suffix_palindrome_lengths()
int count_suffix_palindromes()
int length(int v)
int link(int v)
int diff(int v)
int series_link(int v)
```

`occurrence_count` は `build_occurrences` を内部で呼ぶ。

`diff` と `series_link` は palindromic tree の series link DP 用の値。

## 時間計算量

- 構築: $O(N)$
- occurrence 前計算: $O(V)$
- contains / occurrence_count / find_node: $O(VL)$
- palindromes: 返す文字列長の合計に線形
