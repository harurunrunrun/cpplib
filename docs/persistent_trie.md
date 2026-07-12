---
title: Persistent Trie
documentation_of: ../src/structure/trie/persistent_trie.hpp
---

永続 Trie。重複を持つ。

# テンプレート引数

```cpp
PersistentTrie<ALPHABET, MAX_NODES, MAX_VERSIONS, OFFSET>
```

# version

```cpp
int fork(int version)
int versions()
```

`fork` は同じ根を持つ新しい version を作る。

# insert / erase

```cpp
int insert(int version, string_view s)
int erase(int version, string_view s)
```

新しい version を返す。`erase` は文字列が存在しない場合、同じ内容の新しい version を返す。

# count

```cpp
int count(int version, string_view s)
bool contains(int version, string_view s)
int prefix_count(int version, string_view s)
int size(int version)
```

## 時間計算量

- 文字列長を $L$ として $O(L)$
