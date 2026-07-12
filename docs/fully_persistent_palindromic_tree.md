---
title: Fully Persistent Palindromic Tree
documentation_of: ../src/algorithm/string/fully_persistent_palindromic_tree.hpp
---

任意のversionへ文字を追加できる完全永続Palindromic Tree。

```cpp
FullyPersistentPalindromicTree<ALPHABET, MAX_NODES, MAX_VERSIONS, OFFSET>
```

# update

```cpp
int append(int version, char c)
int add(int version, char c)
```

`version` の文字列末尾へ `c` を追加し、新しいversionを返す。

# query

```cpp
int versions()
int latest_version()
int size(int version)
string str(int version)
int distinct_palindromes(int version)
long long count_palindromic_substrings(int version)
int longest_suffix_palindrome_length(int version)
int longest_palindrome_length(int version)
int find_node(string_view s, int version)
bool contains(string_view s, int version)
vector<int> palindrome_nodes(int version)
vector<string> palindromes(int version)
vector<int> suffix_palindrome_lengths(int version)
```

versionごとの文字列は追加関係を木として保持する。出現済み回文集合は永続segment treeで管理し、別の枝で作られた回文を同じ枝へ追加した場合もdistinct数を正しく更新する。

## 計算量

- `append`: $O(K\log MAX\_VERSIONS + \log MAX\_NODES)$。$K$ はたどるsuffix link数
- longest、count、distinct: $O(1)$
- `contains`: $O(|s| + \log MAX\_NODES)$
- `str`: 文字列長に線形
- `palindrome_nodes`: 返す回文ノード数を $D$ として $O(D\log MAX\_NODES)$
