---
title: Persistent Binary Trie
documentation_of: ../src/structure/trie/persistent_binary_trie.hpp
---

永続 Binary Trie。重複を持つ。

# テンプレート引数

```cpp
PersistentBinaryTrie<T, BIT, MAX_NODES, MAX_VERSIONS>
```

BIT は T のビット幅以下とする。更新が容量不足で失敗した場合、version と node は消費されない。

# version

```cpp
int fork(int version)
int versions()
```

`fork` は同じ根を持つ新しい version を作る。

# insert / erase

```cpp
int insert(int version, T x)
int erase(int version, T x)
```

新しい version を返す。`erase` は値が存在しない場合、同じ内容の新しい version を返す。

# 順序

```cpp
optional<T> kth(int version, int k, T xor_value = 0)
optional<T> min_element(int version, T xor_value = 0)
optional<T> max_element(int version, T xor_value = 0)
int count_less(int version, T x, T xor_value = 0)
```

`xor_value` を指定した場合、値 `y` は `y xor xor_value` の順で比較される。

## 時間計算量

- $O(BIT)$
