---
title: Persistent Palindromic Tree (永続回文木)
documentation_of: ../../../../src/algorithm/string/palindrome/persistent_palindromic_tree.hpp
---

接頭辞ごとの version を持つ回文木である。
`append` するたびに version が 1 つ増え、version `v` は長さ `v` の接頭辞を表す。

## テンプレート引数

```cpp
PersistentPalindromicTree<ALPHABET, MAX_NODES = -1, MAX_LENGTH = -1, OFFSET = 'a'>
```

- `ALPHABET`: アルファベットの文字数（正）。文字は `[OFFSET, OFFSET + ALPHABET)` に属する必要がある。
- `MAX_NODES`: 根 2 個を含むノード数の上限（2 以上）。
- `MAX_LENGTH`: 文字列長の上限。
- `MAX_NODES` と `MAX_LENGTH` のどちらかが `-1`（省略時）なら、実行時サイズに合わせて `std::vector` を伸ばす可変長版になる。

```cpp
PersistentPalindromicTree<26> dynamic(text);
PersistentPalindromicTree<26, 100002, 100000> fixed(text);
```

## 構築

```cpp
PersistentPalindromicTree()
explicit PersistentPalindromicTree(string_view s)
int append(char c)
int add(char c)
```

`append(c)` / `add(c)` は末尾に文字を追加し、新しい接頭辞の version を返す。

## クエリ

```cpp
int versions() const
int size() const
int size(int version) const
int node_count() const
const std::string& str() const
int last(int version) const
int distinct_palindromes(int version) const
long long count_palindromic_substrings(int version) const
int longest_suffix_palindrome_length(int version) const
int longest_palindrome_length(int version) const
int longest_palindrome_node(int version) const
int find_node(string_view s, int version) const
bool contains(string_view s, int version) const
vector<int> palindrome_nodes(int version) const
vector<string> palindromes(int version) const
vector<int> suffix_palindrome_nodes(int version) const
vector<int> suffix_palindrome_lengths(int version) const
```

- version 0 は空文字列である。`versions()` は `size() + 1` を、`str()` は現在の文字列全体への const 参照を返す。
- `size()` は現在の長さ、`size(version)` は指定した接頭辞の長さを返す。
- `last(version)` は最長の回文接尾辞のノードを返す。相異なる回文の個数・回文の出現総数・最長系のクエリは、指定した接頭辞における値を返す。
- `find_node(s, version)` は、`s` が回文で、かつ指定した接頭辞に出現すればノード番号を、なければ `-1` を返す。`contains` はその真偽値版である。空文字列は長さ 0 の根に対応する。
- `palindrome_nodes(version)` / `palindromes(version)` は指定した接頭辞に存在する空でない回文をすべて返す。回文接尾辞の列挙は、指定した接頭辞の回文接尾辞を長い順に返す。

## ノード

```cpp
int length(int node) const
int link(int node) const
int diff(int node) const
int series_link(int node) const
string palindrome(int node) const
pair<int, int> first_occurrence_range(int node) const
```

- `node_count()`、`operator[]`、`length` / `link` / `diff` / `series_link` は、全接頭辞で共有されるノード数・ノード情報を返す。`diff` と `series_link` は series link を用いる DP 用の値である。
- `first_occurrence_range(node)` と `palindrome(node)` は、文字列全体でのノードの初出区間（半開区間）と回文文字列を返す。

## 時間計算量

$V$ を全接頭辞を通じて作られたノード数、$L$ をクエリの文字列長、$D$ を指定 version の相異なる回文の個数、$K$ をその version の回文接尾辞の個数、返す回文文字列の長さの合計を $C$ とする。

| API | 時間計算量 |
| --- | --- |
| `PersistentPalindromicTree()` | 固定長版 $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET}+\texttt{MAX_LENGTH})$ |
| `PersistentPalindromicTree(s)` | 上記の初期化 $+$ 償却 $O(\lvert s\rvert)$ |
| `append(c)` / `add(c)` | 1 回の最悪 $O(\texttt{size()})$、全体で 1 文字あたり償却 $O(1)$（新規ノードの遷移表の初期化 $O(\texttt{ALPHABET})$ を除く） |
| `size()` / `size(version)` / `versions()` / `node_count()` / `str()` | $O(1)$ |
| `operator[]` / `length` / `link` / `diff` / `series_link` | $O(1)$ |
| `last(version)` / `distinct_palindromes(version)` / `count_palindromic_substrings(version)` | $O(1)$ |
| 最長系のクエリ | $O(1)$ |
| `first_occurrence_range(node)` | $O(1)$ |
| `palindrome(node)` | 出力する文字列長に比例 |
| `find_node(s, version)` / `contains(s, version)` | $O(L)$ |
| `palindrome_nodes(version)` | $O(V+D)$、出力 $O(D)$ |
| `palindromes(version)` | $O(V+C)$ |
| `suffix_palindrome_nodes(version)` / `suffix_palindrome_lengths(version)` | $O(K)$ |

`palindrome_nodes` は指定 version にまだ存在しないノードも含め、全 $V$ ノードを走査して選別する。

## 空間計算量

固定長版は $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET}+\texttt{MAX_LENGTH})$、
可変長版は長さ $N$ に対して $O(\texttt{ALPHABET}\cdot N)$ である。

## 注意点

- 追加後の文字数が `MAX_LENGTH` 以下、ノード数が `MAX_NODES` 以下である必要がある。超えると例外を送出する。範囲外の文字も例外を送出する。
- クエリの version は $0\le \mathrm{version}<\texttt{versions()}$ を満たす必要がある。
- 無効なノード番号を渡すと例外を送出する。`first_occurrence_range` と `palindrome` は長さ $-1$ の根に対しても例外を送出する。
