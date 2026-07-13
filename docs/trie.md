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

## API別計算量

$L$ を入力文字列長とする。

- `Trie()`: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES})$。固定長node arrayを初期化する
- `size`, `node_count`, `empty`: $O(1)$
- `node`, `count`, `contains`, `prefix_count`: worst-case $O(L)$
- `insert`: $O(L)$、新規nodeは高々 $L$ 個
- `erase`: $O(L)$。nodeは回収しない

## 空間計算量

- 固定node array: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES})$
- 各操作の追加領域: $O(1)$
