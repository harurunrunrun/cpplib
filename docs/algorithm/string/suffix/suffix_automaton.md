---
title: Suffix Automaton (接尾辞オートマトン)
documentation_of: ../../../../src/algorithm/string/suffix/suffix_automaton.hpp
---

文字列のすべての部分文字列を受理する最小の決定性オートマトン（Suffix Automaton）を構築する。

## テンプレート引数

```cpp
template<int ALPHABET, int MAX_STATES = -1, char OFFSET = 'a'>
struct SuffixAutomaton;
```

- `ALPHABET`: アルファベットの大きさ。文字 `c` は `c - OFFSET` を添字として扱う。
- `MAX_STATES`: 状態数の上限。省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` で状態を確保する。明示すると固定長の配列を使う。
- `OFFSET`: 最小の文字。

```cpp
SuffixAutomaton<26> dynamic(text);
SuffixAutomaton<26, 200001> fixed(text);
```

## 構築

```cpp
SuffixAutomaton()
explicit SuffixAutomaton(std::string_view s)
int extend(char c)
```

## クエリ

```cpp
int size() const
int last() const
const State& operator[](int state) const
bool contains(std::string_view s) const
long long count_distinct_substrings() const
void build_occurrences()
long long occurrence_count(std::string_view s)
std::vector<long long> maximum_occurrences_by_length()
Match longest_common_substring(std::string_view s) const
int longest_common_substring_length(std::span<const std::string_view> additional_strings) const
```

## API 詳細

- `State::next[c]` は文字の添字 `c` で遷移する先の状態、`link` は suffix link、`length` はその状態が表す最長の文字列の長さ、`occurrence` は出現回数である。
- `Match::position`, `Match::length` は、クエリ文字列内における最長共通部分文字列の開始位置と長さである。
- デフォルトコンストラクタは根の状態だけを持つ。`SuffixAutomaton(s)` は `s` の文字を順に追加する。
- `extend(c)` は末尾に 1 文字追加し、新しい末尾の状態を返す。
  文字は `[OFFSET, OFFSET + ALPHABET)` に含まれ、状態数は `MAX_STATES` 以下でなければならない。違反時は `std::runtime_error` を送出する。
- `size()` は使用中の状態数、`last()` は現在の文字列全体に対応する状態を返す。
  `operator[](state)` は状態への const 参照を返し、範囲外では `std::runtime_error` を送出する。
- `contains(s)` は `s` が構築した文字列の部分文字列かを返す。
- `count_distinct_substrings()` は空でない相異なる部分文字列の個数を返す。
- `build_occurrences()` は各状態の終端位置の個数を suffix link 上で集計する。
  `extend` で更新された後は再計算し、更新がなければ再度呼び出しても何もしない。
- `occurrence_count(s)` は部分文字列 `s` の出現回数を返し、現れなければ `0` を返す。必要なら出現回数の前処理も行う。
- `maximum_occurrences_by_length()` は、添字 $i$ に長さ $i+1$ の部分文字列の最大出現回数を格納した列を返す。空文字列では空の列を返す。必要なら出現回数の前処理も行う。
- `longest_common_substring(s)` は構築した文字列と `s` の最長共通部分文字列を `Match` で返す。共通部分がなければ `{0, 0}` を返す。同じ長さの候補が複数あれば、最初に見つかったものを返す。
- `longest_common_substring_length(strings)` は、構築した文字列と `strings` の全要素に共通する最長部分文字列の長さを返す。
  `strings` が空なら構築した文字列の長さ、構築した文字列またはいずれかの要素が空なら `0` を返す。
  追加領域は $O(V+L)$ である。`std::string_view` が参照する文字列は呼び出し中は有効でなければならない。

## API 別の時間計算量・空間計算量

$V$ を使用中の状態数、$L$ を現在の文字列長、$Q$ をクエリ文字列長、$K$ を `strings.size()` とする。
$\texttt{MAX_STATES}$ を指定しない可変長版では、表中の $\texttt{MAX_STATES}$ を $V$ と読み替える。

| API | 時間計算量 |
| --- | --- |
| `SuffixAutomaton()` | $O(\texttt{MAX_STATES} \cdot \texttt{ALPHABET})$（固定配列の初期化） |
| `SuffixAutomaton(s)` | $O(\texttt{MAX_STATES} \cdot \texttt{ALPHABET}+\lvert s\rvert)$ |
| `extend(c)` | 1 回あたり最悪 $O(L)$、一連の追加全体で 1 文字あたり償却 $O(1)$ |
| `size()` / `last()` / `operator[](state)` | $O(1)$ |
| `contains(s)` | $O(Q)$ |
| `count_distinct_substrings()` | $O(V)$ |
| `build_occurrences()` | 更新後の初回は $O(V)$、次の更新までの再呼び出しは $O(1)$ |
| `occurrence_count(s)` | 前処理が必要なら $O(V+Q)$、構築済みなら $O(Q)$ |
| `maximum_occurrences_by_length()` | $O(V+L)$ |
| `longest_common_substring(s)` | 償却 $O(Q)$ |
| `longest_common_substring_length(strings)` | $O(V(K+1)+\sum_i \lvert \mathtt{strings}_i\rvert)$ |

`State` と `Match` の各メンバの参照は $O(1)$ である。
保持する領域は $O(\texttt{MAX_STATES} \cdot \texttt{ALPHABET})$（可変長版では $O(V \cdot \texttt{ALPHABET})$）である。
可変長版の構築は、状態の生成に伴う `std::vector` の再配置を含めて、長さ $N$ に対し償却 $O(N \cdot \texttt{ALPHABET})$ 時間・空間である。

## 注意点

- 返す位置はすべて 0-indexed である。
- 同じヘッダ内で、バイト文字列向けの API が内部で利用する automaton（`suffix_automaton_internal::ByteSuffixAutomaton`）は状態の ID を `int` で保持する。
  入力長 $N$ は `(std::numeric_limits<int>::max() / 2) + 1` 以下でなければならず、超える入力は領域の確保や状態の追加より前に `std::length_error` で拒否する。
  状態数の上限は $N = 1$ のとき $2$、それ以外の空でない入力では $2N - 1$ である。
