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
int node_count()
int root(int version)
```

`fork` は同じ根を持つ新しい version を作る。
`versions`はversion数、`node_count`は使用済みnode数を返す。
`root`は指定versionのroot node番号を返す。


更新が容量不足で失敗した場合、version と node は消費されない。
`version`が`[0,versions())`の範囲外なら例外。
`fork`、`insert`、`erase`でversion数が`MAX_VERSIONS`を超える場合も例外。

# node

```cpp
int node(int version, string_view s) const
```

`version`で文字列`s`に対応するnode番号を返す。存在しなければ`-1`。
文字は`[OFFSET, OFFSET + ALPHABET)`に含まれる必要があり、範囲外なら例外。


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

`count`は完全一致数、`contains`は完全一致の有無、
`prefix_count`は接頭辞`s`を持つ登録文字列数、`size`は重複込み総登録数を返す。
`insert`でnode容量を超える場合は例外。`erase`対象が存在しない場合は
nodeを生成せず、同じ内容のversionだけを作る。

- `PersistentTrie()`: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- `versions`, `node_count`, `root`, `size`, `fork`: $O(1)$。`fork` はnodeを増やさずversionだけを生成する
- `node`, `count`, `contains`, `prefix_count`: worst-case $O(L)$
- `insert`: $O(L)$、path-copy node $L+1$ 個と1 versionを生成する
- `erase`: $O(L)$。存在すればpath-copy node $L+1$ 個、存在しなければ0個。どちらも1 versionを生成する

## 空間計算量

- 固定node/version array: $O(\mathrm{ALPHABET}\cdot\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- 各操作の追加stack: $O(1)$
