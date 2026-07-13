---
title: Binary Trie
documentation_of: ../src/structure/trie/binary_trie.hpp
---

整数の Binary Trie。重複を持つ。

# テンプレート引数

```cpp
BinaryTrie<T, BIT, MAX_NODES>
```

- 整数型 `T`
- bit 数 `BIT`
- ノード数の上限 `MAX_NODES`

# insert / erase

```cpp
void insert(T x)
bool erase(T x)
```

`erase` は値が存在しない場合 `false` を返す。

# count

```cpp
int count(T x)
bool contains(T x)
int size()
```

# 順序

```cpp
optional<T> kth(int k, T xor_value = 0)
optional<T> min_element(T xor_value = 0)
optional<T> max_element(T xor_value = 0)
int count_less(T x, T xor_value = 0)
```

`xor_value` を指定した場合、値 `y` は `y xor xor_value` の順で比較される。

## API別計算量

$B=\mathrm{BIT}$ とする。

- `BinaryTrie()`: $O(\mathrm{MAX\_NODES})$
- `size`, `node_count`, `empty`: $O(1)$
- `insert`, `count`, `contains`, `erase`: $O(B)$。`insert` の新規nodeは高々 $B$ 個
- `kth`, `min_element`, `max_element`, `count_less`: $O(B)$

## 空間計算量

- 固定node array: $O(\mathrm{MAX\_NODES})$
- 各操作の追加領域: $O(1)$
