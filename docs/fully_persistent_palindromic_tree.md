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

## API別計算量

$H_V=\log MAX_VERSIONS$、$H_N=\log MAX_NODES$、`append` がたどるsuffix link数を $K$ とする。

| API | 時間計算量 |
| --- | --- |
| `FullyPersistentPalindromicTree()` | $O(MAX_NODES\cdot ALPHABET+MAX_VERSIONS(H_N+H_V))$ |
| `append(version,c)` / `add(version,c)` | $O((K+1)H_V+H_N+ALPHABET)$ |
| `append(c)` / `add(c)` | 上と同じ（latest versionへ追加） |
| `versions()` / `latest_version()` / `node_count()` / `size(version)` / `size()` | $O(1)$ |
| `operator[]` / `length` / `link` / `diff` / `series_link` | $O(1)$ |
| `str(version)` | $O(S)$（version文字列長 $S$ の列挙） |
| `last` / `distinct_palindromes` / `count_palindromic_substrings` | $O(1)$ |
| longest系query | $O(1)$ |
| `node_present(version,node)` | $O(H_N)$ |
| `find_node(s,version)` / `contains(s,version)` | $O(|s|+H_N)$ |
| `first_occurrence_range(node)` | $O(1)$ |
| `palindrome(node)` | $O(F)$（node初出versionの長さ $F$ を復元） |
| `palindrome_nodes(version)` | $O(DH_N)$、出力 $O(D)$ |
| `palindromes(version)` | $O(DH_N+\sum F_i)$ |
| `suffix_palindrome_nodes(version)` / `suffix_palindrome_lengths(version)` | $O(K)$ |

constructorはbinary-lifting表と永続set領域も初期化する。`append` はbranch間で償却せず、その呼出しでたどる $K$ を明示する。保持領域は $O(MAX_NODES\cdot ALPHABET+MAX_VERSIONS(H_N+H_V))$。
