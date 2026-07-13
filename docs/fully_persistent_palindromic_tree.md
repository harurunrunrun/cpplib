---
title: Fully Persistent Palindromic Tree (完全永続回文木)
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

## 計算量の概要

- `append`: $O(K\log MAX\_VERSIONS + \log MAX\_NODES)$。$K$ はたどるsuffix link数
- longest、count、distinct: $O(1)$
- `contains`: $O(|s| + \log MAX\_NODES)$
- `str`: 文字列長に線形
- `palindrome_nodes`: 返す回文ノード数を $D$ として $O(D\log MAX\_NODES)$

## API別の時間計算量・空間計算量

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
## 注意点

- version 0は空文字列。`versions()` は存在version数、
  `latest_version()` は最後に作成したversion番号、`node_count()` はroot込みnode数。
- `append(version,c)` / `add(version,c)` は指定versionの末尾へ追加した新versionを返す。
  引数なしversion overloadは最新版へ追加する。元versionは変更しない。
- versionは $0\le version<versions()$、文字はalphabet内である必要がある。
  `MAX_VERSIONS`、`MAX_NODES` または内部永続set容量を超える追加は例外。
- `size(version)`, `str(version)`, `last(version)` はそれぞれ文字列長、復元文字列、
  最長回文suffix node。`size()` は最新版を対象にする。
- `operator[](node)` と `length/link/diff/series_link(node)` は全versionで共有される
  node情報を返す。無効nodeでは例外。
- `Node::next/link/length/diff/series_link` は通常版と同じ意味。
  `suffix_count` はその回文の回文suffix数、`first_version` は初出version。
- `distinct_palindromes(version)`、`count_palindromic_substrings(version)` は
  指定versionの異なる回文数・出現を位置別に数えた総数。
- longest系queryは指定versionに存在する最長回文または最長回文suffixを返す。
- `node_present(version,node)` はnodeの回文がそのversionに出現するかを返す。
  2つのrootは常に存在するとみなす。
- `find_node(value,version)` は回文が指定versionに存在すればnode、なければ-1。
  `contains` はそのbool版。
- `first_occurrence_range(node)`, `palindrome(node)` はnodeが初めて作られた枝での
  最初の出現位置・文字列を返す。長さ-1のrootでは例外。
- `palindrome_nodes/palindromes(version)` は指定versionに存在する全非空回文、
  `suffix_palindrome_nodes/lengths(version)` は回文suffixを長い順に返す。
