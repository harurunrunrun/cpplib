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

更新が容量不足で失敗した場合、version と node は消費されない。

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

## API別計算量

$L$ を入力文字列長とする。

- `PersistentTrie()`: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- `versions`, `node_count`, `root`, `size`, `fork`: $O(1)$。`fork` はnodeを増やさずversionだけを生成する
- `node`, `count`, `contains`, `prefix_count`: worst-case $O(L)$
- `insert`: $O(L)$、path-copy node $L+1$ 個と1 versionを生成する
- `erase`: $O(L)$。存在すればpath-copy node $L+1$ 個、存在しなければ0個。どちらも1 versionを生成する

## 空間計算量

- 固定node/version array: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- 各操作の追加stack: $O(1)$
