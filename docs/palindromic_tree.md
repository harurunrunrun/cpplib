---
title: Palindromic Tree (回文木)
documentation_of: ../src/algorithm/string/palindrome/palindromic_tree.hpp
---

Palindromic Tree。

## テンプレート引数

```cpp
PalindromicTree<ALPHABET, MAX_NODES, OFFSET>
```

## construct

```cpp
PalindromicTree()
PalindromicTree(string_view s)
int add(char c)
```

## query

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
int parent(int v)
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

## API別の時間計算量・空間計算量

$N$ を現在長、$V$ を使用node数、$L$ をquery回文長、$D=V-2$、$K$ をsuffix回文数、列挙する回文文字列の総長を $C$ とする。

| API | 時間計算量 |
| --- | --- |
| `PalindromicTree()` | $O(MAX_NODES\cdot ALPHABET)$（固定node配列初期化） |
| `PalindromicTree(s)` | $O(MAX_NODES\cdot ALPHABET+\lvert s\rvert)$ 償却 |
| `add(c)` | 1回の最悪 $O(N)$、一連の末尾追加全体で償却 $O(1)$ / 文字 |
| `size()` / `node_count()` / `distinct_palindromes()` / `last()` | $O(1)$ |
| `operator[]` / `length` / `parent` / `link` / `diff` / `series_link` / `str()` | $O(1)$（参照・metadata） |
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

## API詳細

### `Node`

- `next[c]`: 文字index `c` を両端へ加えて得る回文node。存在しなければ-1。
- `parent`: 対応回文の先頭・末尾を1文字ずつ除いた回文node。
- `link`: 最長の真の回文suffix、`length`: 回文長。
- `diff`, `series_link`: series-link DP用の長さ差と遷移先。
- `terminal_occurrence`: 追加処理中にそのnodeが最長suffixになった回数。
- `occurrence`: `build_occurrences()` 後の文字列全体での出現回数。
- `first_position`: 最初の出現の右端index。

### 構築・metadata

- default constructorは長さ-1と0の2つのrootだけを構築する。
  文字列constructorは各文字を `add` する。
- `add(c)` は末尾へ文字を追加し、新しい最長回文suffixのnode番号を返す。
  `c` はalphabet内、node数は `MAX_NODES` 以下が必要。
- `size()` は文字列長、`node_count()` はroot込みnode数、
  `distinct_palindromes()` は非root node数、`last()` は最長回文suffix node。
- `operator[](v)` はnodeへのconst参照。`length/parent/link/diff/series_link` は
  対応fieldを返し、無効nodeでは例外。`str()` は構築文字列へのconst参照。

### 出現・検索

- `build_occurrences()` はsuffix link上へ出現数を伝播する。更新後だけ再計算する。
- `count_palindromic_substrings()` は位置の異なる出現をすべて数える。
- `find_node(s)` は `s` が回文かつ出現済みならnode番号、そうでなければ-1。
  空文字列は長さ0のroot node 1。
- `contains(s)` は `find_node(s)!=-1`。
- `occurrence_count(s)` は出現回数。空文字列は `size()+1` 回と数える。

### 復元・列挙

- `first_occurrence_range(v)` は最初の出現を半開区間で返す。
  長さ-1のrootでは例外、長さ0のrootでは `{0,0}`。
- `palindrome(v)` はnodeが表す文字列を返す。
- `palindrome_nodes()`, `palindromes()` は全非空回文をnode番号順に返す。
- `suffix_palindrome_nodes()`, `suffix_palindrome_lengths()` は現在の文字列の
  回文suffixを長い順に返す。`count_suffix_palindromes()` はその個数。
- `longest_suffix_palindrome_length()` は現在末尾の最長回文長。
  `longest_palindrome_node/length` は文字列全体の最長回文を返す。

## 注意点

文字範囲、alphabet、固定容量、構築後の更新可否は各APIの説明に従う。返すindexと区間は0-indexedである。
