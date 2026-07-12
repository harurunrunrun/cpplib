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
