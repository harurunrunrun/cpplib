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
