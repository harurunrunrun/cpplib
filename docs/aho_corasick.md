---
title: Aho Corasick
documentation_of: ../src/algorithm/string/aho_corasick.hpp
---

複数パターンマッチング。

# テンプレート引数

```cpp
AhoCorasick<ALPHABET, MAX_NODES, OFFSET>
```

- 文字種類数 `ALPHABET`
- ノード数の上限 `MAX_NODES`
- 先頭文字 `OFFSET`

# add / build

```cpp
int add(string_view pattern)
void build()
```

パターンを追加してから `build` する。

# query

```cpp
long long count_matches(string_view text)
int next_state(int state, char c)
int terminal_count(int state)
int output_count(int state)
```

`count_matches` は text に含まれる全パターンの出現回数の総和を返す。

`output_count` はその状態で終わるパターン数を fail link 分も含めて返す。

## 時間計算量

- build: $O(\Sigma V)$
- query: $O(|S|)$
