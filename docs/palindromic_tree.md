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
long long count_palindromic_substrings()
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

- 固定node配列初期化後の文字列構築: 償却 $O(N)$
- occurrence 前計算: $O(V)$
- contains / occurrence_count / find_node: $O(L)$
- palindromes: 返す文字列長の合計に線形

## API別計算量

$N$ を現在長、$V$ を使用node数、$L$ をquery回文長、$D=V-2$、$K$ をsuffix回文数、列挙する回文文字列の総長を $C$ とする。

| API | 時間計算量 |
| --- | --- |
| `PalindromicTree()` | $O(MAX_NODES\cdot ALPHABET)$（固定node配列初期化） |
| `PalindromicTree(s)` | $O(MAX_NODES\cdot ALPHABET+|s|)$ 償却 |
| `add(c)` | 1回の最悪 $O(N)$、一連の末尾追加全体で償却 $O(1)$ / 文字 |
| `size()` / `node_count()` / `distinct_palindromes()` / `last()` | $O(1)$ |
| `operator[]` / `length` / `link` / `diff` / `series_link` / `str()` | $O(1)$（参照・metadata） |
| `build_occurrences()` | 更新後の初回 $O(V)$、再更新までの再呼出し $O(1)$ |
| `count_palindromic_substrings()` | $O(V)$（必要なら同時に前処理） |
| `longest_suffix_palindrome_length()` | $O(1)$ |
| `find_node(s)` / `contains(s)` | $O(L)$ |
| `occurrence_count(s)` | 前処理済みなら $O(L)$、未処理なら $O(V+L)$ |
| `first_occurrence_range(v)` | $O(1)$ |
| `palindrome(v)` | $O(\ell_v)$（出力文字列長） |
| `palindrome_nodes()` | $O(D)$ |
| `palindromes()` | $O(V+C)$ |
| `suffix_palindrome_nodes()` / `suffix_palindrome_lengths()` / `count_suffix_palindromes()` | $O(K)$ |
| `longest_palindrome_node()` / `longest_palindrome_length()` | $O(V)$ |

`Node` の各field参照は $O(1)$。保持領域は $O(MAX_NODES\cdot ALPHABET+N)$。
