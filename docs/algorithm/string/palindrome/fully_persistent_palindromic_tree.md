---
title: Fully Persistent Palindromic Tree (完全永続回文木)
documentation_of: ../../../../src/algorithm/string/palindrome/fully_persistent_palindromic_tree.hpp
---

任意の version に文字を追加できる、完全永続の回文木（Palindromic Tree）である。

```cpp
FullyPersistentPalindromicTree<ALPHABET, MAX_NODES = -1, MAX_VERSIONS = -1, OFFSET = 'a'>
```

- `ALPHABET`: アルファベットの文字数（正）。文字は `[OFFSET, OFFSET + ALPHABET)` に属する必要がある。
- `MAX_NODES`: 根 2 個を含むノード数の上限（2 以上）。
- `MAX_VERSIONS`: `append` の回数の上限。version 0 を含めて最大 $\texttt{MAX_VERSIONS}+1$ 個の version を持てる。
- `MAX_NODES` と `MAX_VERSIONS` のどちらかが `-1`（省略時）なら、実行時サイズに合わせて `std::vector` を伸ばす可変長版になる。

```cpp
FullyPersistentPalindromicTree<26> dynamic;
FullyPersistentPalindromicTree<26, 100002, 100001> fixed;
```

各 version の文字列は、追加関係を木（version 木）として保持する。各 version に出現済みの回文の集合は永続セグメント木で管理するため、
別の枝で作られた回文を同じ枝に追加した場合も、相異なる回文の個数を正しく更新できる。

## 更新

```cpp
int append(int version, char c)
int add(int version, char c)
int append(char c)
int add(char c)
```

- `append(version, c)` / `add(version, c)` は、`version` の文字列の末尾に `c` を追加した新しい version を作り、その番号を返す。元の version は変更しない。
- version を省略した版は最新の version に追加する。

## クエリ

```cpp
int versions() const
int latest_version() const
int node_count() const
int size(int version) const
int size() const
std::string str(int version) const
int last(int version) const
int distinct_palindromes(int version) const
long long count_palindromic_substrings(int version) const
int longest_suffix_palindrome_length(int version) const
int longest_palindrome_node(int version) const
int longest_palindrome_length(int version) const
bool node_present(int version, int node) const
int find_node(std::string_view s, int version) const
bool contains(std::string_view s, int version) const
std::pair<int, int> first_occurrence_range(int node) const
std::string palindrome(int node) const
std::vector<int> palindrome_nodes(int version) const
std::vector<std::string> palindromes(int version) const
std::vector<int> suffix_palindrome_nodes(int version) const
std::vector<int> suffix_palindrome_lengths(int version) const
```

- version 0 は空文字列である。`versions()` は存在する version の個数、`latest_version()` は最後に作った version の番号、`node_count()` は根を含むノード数を返す。
- `size(version)`、`str(version)`、`last(version)` は、それぞれ文字列長、復元した文字列、最長の回文接尾辞のノードを返す。`size()` は最新の version を対象にする。
- `operator[](node)` と `length` / `link` / `diff` / `series_link(node)` は、全 version で共有されるノード情報を返す。`Node` の `next` / `link` / `length` / `diff` / `series_link` は通常の回文木と同じ意味であり、`suffix_count` はその回文の回文接尾辞の個数、`first_version` は初めて作られた version である。`diff` と `series_link` は series link を用いる DP 用の値である。
- `distinct_palindromes(version)` は指定 version の相異なる回文の個数、`count_palindromic_substrings(version)` は出現位置ごとに数えた回文の総数を返す。
- 最長系のクエリは、指定 version に存在する最長の回文、または最長の回文接尾辞を返す。
- `node_present(version, node)` は、ノードが表す回文がその version に出現するかを返す。2 つの根は常に存在するとみなす。
- `find_node(s, version)` は、回文 `s` が指定 version に存在すればそのノード番号を、なければ `-1` を返す。`contains` はその真偽値版である。
- `first_occurrence_range(node)` と `palindrome(node)` は、ノードが初めて作られた枝での最初の出現位置（半開区間）と回文文字列を返す。
- `palindrome_nodes(version)` / `palindromes(version)` は指定 version に存在する空でない回文をすべて返し、`suffix_palindrome_nodes(version)` / `suffix_palindrome_lengths(version)` は回文接尾辞を長い順に返す。

## 時間計算量

$H_V=\log \texttt{MAX_VERSIONS}$、$H_N=\log \texttt{MAX_NODES}$ とし（可変長版では version 数 $V$・ノード数 $S$ に対する $\log(V+S)$）、
`append` がたどる接尾辞リンクの本数を $K$、指定 version に存在する回文の個数を $D$ とする。

| API | 時間計算量 |
| --- | --- |
| `FullyPersistentPalindromicTree()` | 固定長版 $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET}+\texttt{MAX_VERSIONS}(H_N+H_V))$ |
| `append(version, c)` / `add(version, c)` / `append(c)` / `add(c)` | $O((K+1)H_V+H_N+\texttt{ALPHABET})$ |
| `versions()` / `latest_version()` / `node_count()` / `size(version)` / `size()` | $O(1)$ |
| `operator[]` / `length` / `link` / `diff` / `series_link` | $O(1)$ |
| `str(version)` | $O(S)$（$S$ はその version の文字列長） |
| `last` / `distinct_palindromes` / `count_palindromic_substrings` | $O(1)$ |
| 最長系のクエリ | $O(1)$ |
| `node_present(version, node)` | $O(H_N)$ |
| `find_node(s, version)` / `contains(s, version)` | $O(\lvert s\rvert+H_N)$ |
| `first_occurrence_range(node)` | $O(1)$ |
| `palindrome(node)` | $O(F)$（ノードが初めて現れた version の長さ $F$ から復元する） |
| `palindrome_nodes(version)` | $O(DH_N)$、出力 $O(D)$ |
| `palindromes(version)` | $O(DH_N+\sum F_i)$ |
| `suffix_palindrome_nodes(version)` / `suffix_palindrome_lengths(version)` | $O(K)$ |

コンストラクタは binary lifting の表と永続集合の領域も初期化する。`append` は枝をまたいで償却できないため、各呼び出しでたどる $K$ を明示している。

## 空間計算量

固定長版は $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET}+\texttt{MAX_VERSIONS}(H_N+H_V))$、
可変長版は $A$ を `ALPHABET` として $O(AS+V\log(V+S))$ である。

## 注意点

- version は $0\le \mathrm{version}<\texttt{versions()}$、文字はアルファベット内である必要がある。
- `MAX_VERSIONS`、`MAX_NODES`、または内部の永続集合の容量を超える追加では例外を送出する。
- 無効なノード番号を渡すと例外を送出する。`first_occurrence_range` と `palindrome` は長さ $-1$ の根に対しても例外を送出する。
