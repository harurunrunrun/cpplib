---
title: Trie
documentation_of: ../src/structure/trie/trie.hpp
---

文字列の Trie。重複を持つ。

# テンプレート引数

```cpp
Trie<ALPHABET, MAX_NODES, OFFSET>
```

- 文字種類数 `ALPHABET`
- ノード数の上限 `MAX_NODES`
- 先頭文字 `OFFSET`

# insert / erase

```cpp
void insert(string_view s)
bool erase(string_view s)
```

`erase` は文字列が存在しない場合 `false` を返す。

# count

```cpp
int count(string_view s)
bool contains(string_view s)
int prefix_count(string_view s)
int node(string_view s)
```

`count` は文字列の個数を返す。

`prefix_count` は prefix を持つ文字列の個数を返す。

`node` は対応するノード番号を返す。存在しない場合 `-1` を返す。

## 時間計算量

- 文字列長を $L$ として $O(L)$
