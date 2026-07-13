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

## API別計算量

$V$ を全prefixで作られたnode数、$L$ をquery長、$D$ を指定versionのdistinct回文数、$K$ をそのversionのsuffix回文数、返す回文文字列総長を $C$ とする。

| API | 時間計算量 |
| --- | --- |
| `PersistentPalindromicTree()` | $O(MAX_NODES\cdot ALPHABET+MAX_LENGTH)$ |
| `PersistentPalindromicTree(s)` | 上記初期化 $+O(|s|)$ 償却 |
| `append(c)` / `add(c)` | 1回の最悪 $O(size())$、全prefix追加で償却 $O(1)$ / 文字 |
| `size()` / `size(version)` / `versions()` / `node_count()` / `str()` | $O(1)$ |
| `operator[]` / `length` / `link` / `diff` / `series_link` | $O(1)$ |
| `last(version)` / `distinct_palindromes(version)` | $O(1)$ |
| `count_palindromic_substrings(version)` | $O(1)$ |
| longest系query | $O(1)$ |
| `first_occurrence_range(node)` | $O(1)$ |
| `palindrome(node)` | $O(\ell_{node})$（出力文字列長） |
| `find_node(s,version)` / `contains(s,version)` | $O(L)$ |
| `palindrome_nodes(version)` | $O(V+D)$、出力 $O(D)$ |
| `palindromes(version)` | $O(V+C)$ |
| `suffix_palindrome_nodes(version)` / `suffix_palindrome_lengths(version)` | $O(K)$ |

`palindrome_nodes` は指定versionにまだ存在しないnodeも含め、全 $V$ nodeを走査して選別する。保持領域は $O(MAX_NODES\cdot ALPHABET+MAX_LENGTH)$。
