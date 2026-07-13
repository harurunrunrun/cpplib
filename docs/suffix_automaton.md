---
title: Suffix Automaton
documentation_of: ../src/algorithm/string/suffix_automaton.hpp
---

Suffix Automaton。

# テンプレート引数

```cpp
SuffixAutomaton<ALPHABET, MAX_STATES, OFFSET>
```

# construct

```cpp
SuffixAutomaton()
SuffixAutomaton(string_view s)
int extend(char c)
```

# query

```cpp
bool contains(string_view s)
long long count_distinct_substrings()
void build_occurrences()
long long occurrence_count(string_view s)
Match longest_common_substring(string_view s)
```

`Match` は `position` と `length` を持つ。

## 時間計算量

- 構築: $O(N)$
- contains: $O(|S|)$
- occurrence 前計算: $O(V)$
- longest common substring: $O(|S|)$

## API別計算量

$V$ を使用中state数、$L$ を現在の文字列長、$Q$ をquery文字列長とする。

| API | 時間計算量 |
| --- | --- |
| `SuffixAutomaton()` | $O(MAX_STATES\cdot ALPHABET)$（固定配列初期化） |
| `SuffixAutomaton(s)` | $O(MAX_STATES\cdot ALPHABET+|s|)$ |
| `extend(c)` | 1回の最悪 $O(L)$、一連の追加全体で償却 $O(1)$ / 文字 |
| `size()` / `last()` / `operator[](state)` | $O(1)$（metadata/state参照） |
| `contains(s)` | $O(Q)$ |
| `count_distinct_substrings()` | $O(V)$ |
| `build_occurrences()` | 更新後の初回 $O(V)$、再更新までの再呼出し $O(1)$ |
| `occurrence_count(s)` | 前処理が必要なら $O(V+Q)$、構築済みなら $O(Q)$ |
| `longest_common_substring(s)` | 償却 $O(Q)$ |

`State::next/link/length/occurrence` と `Match::position/length` の参照は $O(1)$。保持領域は $O(MAX_STATES\cdot ALPHABET)$。
## API詳細

- `State::next[c]` は文字indexで進むstate、`link` はsuffix link、
  `length` はそのstateが表す最長文字列長、`occurrence` は出現回数。
- `Match::position`, `length` はquery文字列内の最長共通部分文字列の開始位置と長さ。
- default constructorはroot stateだけを持つ。`SuffixAutomaton(s)` は `s` を順に追加する。
- `extend(c)` は末尾へ1文字追加し、新しい末尾stateを返す。
  文字は `[OFFSET,OFFSET+ALPHABET)`、state数は `MAX_STATES` 以下が必要。
- `size()` は使用state数、`last()` は現在の文字列全体に対応するstate。
  `operator[](state)` はstateへのconst参照で、範囲外では例外。
- `contains(s)` は `s` が構築文字列の部分文字列かを返す。
  `count_distinct_substrings()` は異なる非空部分文字列数を返す。
- `build_occurrences()` は各stateのend-position数をsuffix link上で集計する。
  更新後に再計算され、更新がなければ再呼出しは何もしない。
- `occurrence_count(s)` は部分文字列 `s` の出現回数を返し、
  未出現なら0。必要ならoccurrence前処理も行う。
- `longest_common_substring(s)` は構築文字列と `s` の最長共通部分文字列を
  `Match` で返す。空の場合は `{0,0}`。同長候補では最初に更新されたものを返す。
