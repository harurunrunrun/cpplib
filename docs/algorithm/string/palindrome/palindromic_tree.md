---
title: Palindromic Tree (回文木)
documentation_of: ../../../../src/algorithm/string/palindrome/palindromic_tree.hpp
---

回文木（Palindromic Tree, eertree）である。文字列の末尾に文字を追加しながら、相異なる回文部分文字列を管理する。

## テンプレート引数

```cpp
PalindromicTree<ALPHABET, MAX_NODES = -1, OFFSET = 'a'>
```

- `ALPHABET`: アルファベットの文字数（正）。文字は `[OFFSET, OFFSET + ALPHABET)` に属する必要がある。
- `MAX_NODES`: 根 2 個を含むノード数の上限（2 以上）。省略時の `-1` は、実行時サイズに合わせて `std::vector` を伸ばす可変長版を表す。

```cpp
PalindromicTree<26> dynamic(text);
PalindromicTree<26, 100002> fixed(text);
```

## 構築

```cpp
PalindromicTree()
explicit PalindromicTree(string_view s)
int add(char c)
```

- デフォルトコンストラクタは長さ $-1$ と $0$ の 2 つの根だけを構築する。文字列を受け取るコンストラクタは各文字を `add` する。
- `add(c)` は末尾に文字を追加し、新しい最長の回文接尾辞のノード番号を返す。

## クエリ

```cpp
int size() const
int node_count() const
int distinct_palindromes() const
int last() const
const std::string& str() const
int longest_suffix_palindrome_length() const
int longest_palindrome_length() const
int longest_palindrome_node() const
void build_occurrences()
long long count_palindromic_substrings()
int find_node(string_view s) const
bool contains(string_view s) const
int occurrence_count(string_view s)
string palindrome(int v) const
pair<int, int> first_occurrence_range(int v) const
vector<int> palindrome_nodes() const
vector<string> palindromes() const
vector<int> suffix_palindrome_nodes() const
vector<int> suffix_palindrome_lengths() const
int count_suffix_palindromes() const
int length(int v) const
int parent(int v) const
int link(int v) const
int diff(int v) const
int series_link(int v) const
```

### `Node`

- `next[c]`: 文字番号 `c` を両端に加えて得られる回文のノード。存在しなければ `-1`。
- `parent`: 対応する回文の先頭と末尾を 1 文字ずつ除いた回文のノード。
- `link`: 最長の真の回文接尾辞のノード。`length`: 回文の長さ。
- `diff`, `series_link`: series link を用いる DP 用の長さの差と遷移先。
- `terminal_occurrence`: 追加処理中にそのノードが最長の回文接尾辞になった回数。
- `occurrence`: `build_occurrences()` 後の、文字列全体での出現回数。
- `first_position`: 最初の出現の右端の添字。

### 基本情報

- `size()` は文字列長、`node_count()` は根を含むノード数、`distinct_palindromes()` は根以外のノード数、`last()` は最長の回文接尾辞のノードを返す。
- `operator[](v)` はノードへの const 参照を返す。`length` / `parent` / `link` / `diff` / `series_link` は対応するフィールドを返す。`str()` は構築した文字列への const 参照を返す。

### 出現・検索

- `build_occurrences()` は接尾辞リンクに沿って出現回数を伝播する。更新があったときだけ再計算する。
- `count_palindromic_substrings()` は位置の異なる出現をすべて数える。
- `find_node(s)` は、`s` が回文で、かつ出現済みならノード番号を、そうでなければ `-1` を返す。空文字列には長さ 0 の根であるノード 1 を返す。
- `contains(s)` は `find_node(s) != -1` を返す。
- `occurrence_count(s)` は出現回数を返す。必要なら `build_occurrences` を内部で呼ぶ。空文字列は `size() + 1` 回と数える。

### 復元・列挙

- `first_occurrence_range(v)` は最初の出現を半開区間で返す。長さ 0 の根では `{0, 0}` を返す。
- `palindrome(v)` はノードが表す文字列を返す。
- `palindrome_nodes()`, `palindromes()` は空でない回文をすべてノード番号順に返す。
- `suffix_palindrome_nodes()`, `suffix_palindrome_lengths()` は現在の文字列の回文接尾辞を長い順に返す。`count_suffix_palindromes()` はその個数を返す。
- `longest_suffix_palindrome_length()` は現在の末尾で終わる最長の回文の長さを返す。`longest_palindrome_node()` / `longest_palindrome_length()` は文字列全体の最長回文を返す。

## 時間計算量

$N$ を現在の文字列長、$V$ を使用ノード数、$L$ をクエリの回文長、$D=V-2$、$K$ を回文接尾辞の個数、列挙する回文文字列の長さの合計を $C$ とする。

| API | 時間計算量 |
| --- | --- |
| `PalindromicTree()` | 固定長版 $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET})$（固定ノード配列の初期化）、可変長版 $O(\texttt{ALPHABET})$ |
| `PalindromicTree(s)` | 上記の初期化 $+$ 償却 $O(\texttt{ALPHABET}\cdot\lvert s\rvert)$（`ALPHABET` を定数とみなせば $O(\lvert s\rvert)$） |
| `add(c)` | 1 回の最悪 $O(N+\texttt{ALPHABET})$、末尾追加の列全体で 1 文字あたり償却 $O(\texttt{ALPHABET})$ |
| `size()` / `node_count()` / `distinct_palindromes()` / `last()` | $O(1)$ |
| `operator[]` / `length` / `parent` / `link` / `diff` / `series_link` / `str()` | $O(1)$ |
| `build_occurrences()` | 更新後の初回 $O(V)$、次の更新までの再呼び出しは $O(1)$ |
| `count_palindromic_substrings()` | $O(V)$（必要なら前処理も行う） |
| `longest_suffix_palindrome_length()` | $O(1)$ |
| `find_node(s)` / `contains(s)` | $O(L)$ |
| `occurrence_count(s)` | 前処理済みなら $O(L)$、未処理なら $O(V+L)$ |
| `first_occurrence_range(v)` | $O(1)$ |
| `palindrome(v)` | 出力する文字列長に比例 |
| `palindrome_nodes()` | $O(D)$ |
| `palindromes()` | $O(V+C)$ |
| `suffix_palindrome_nodes()` / `suffix_palindrome_lengths()` / `count_suffix_palindromes()` | $O(K)$ |
| `longest_palindrome_node()` / `longest_palindrome_length()` | $O(V)$ |

## 空間計算量

固定長版は $O(\texttt{MAX_NODES}\cdot\texttt{ALPHABET}+N)$、可変長版は $O(\texttt{ALPHABET}\cdot N)$ である。

## 注意点

- 範囲外の文字や、ノード数が `MAX_NODES` を超える追加では例外を送出する。
- 無効なノード番号を `operator[]` や `length` などに渡すと例外を送出する。`first_occurrence_range` は長さ $-1$ の根に対しても例外を送出する。
- 返す添字と区間は 0-indexed である。
