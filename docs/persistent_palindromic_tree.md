---
title: Persistent Palindromic Tree
documentation_of: ../src/algorithm/string/persistent_palindromic_tree.hpp
---

prefix version を持つ Palindromic Tree。

`append` するたびに 1 つ version が増える。version `v` は長さ `v` の prefix を表す。

# テンプレート引数

```cpp
PersistentPalindromicTree<ALPHABET, MAX_NODES, MAX_LENGTH, OFFSET>
```

# construct

```cpp
PersistentPalindromicTree()
PersistentPalindromicTree(string_view s)
int append(char c)
int add(char c)
```

`append` / `add` は追加後の version を返す。

# query

```cpp
int versions()
int size()
int size(int version)
int distinct_palindromes(int version)
long long count_palindromic_substrings(int version)
int longest_suffix_palindrome_length(int version)
int longest_palindrome_length(int version)
int longest_palindrome_node(int version)
int find_node(string_view s, int version)
bool contains(string_view s, int version)
vector<int> palindrome_nodes(int version)
vector<string> palindromes(int version)
vector<int> suffix_palindrome_nodes(int version)
vector<int> suffix_palindrome_lengths(int version)
```

# node

```cpp
int length(int node)
int link(int node)
int diff(int node)
int series_link(int node)
string palindrome(int node)
pair<int,int> first_occurrence_range(int node)
```

`diff` と `series_link` は palindromic tree の series link DP 用の値。

## 時間計算量

- `append`: amortized $O(1)$
- `distinct_palindromes`, `count_palindromic_substrings`, longest 系: $O(1)$
- `contains`, `find_node`: $O(L)$
- `palindromes`: 返す文字列長の合計に線形
