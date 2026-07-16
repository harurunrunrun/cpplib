---
title: Persistent Binary Trie (完全永続二進トライ)
documentation_of: ../src/structure/trie/persistent_binary_trie.hpp
---

永続 Binary Trie。重複を持つ。

## テンプレート引数

```cpp
PersistentBinaryTrie<T, BIT, MAX_NODES, MAX_VERSIONS>
```

BIT は T のビット幅以下とする。更新が容量不足で失敗した場合、version と node は消費されない。

## version

```cpp
int fork(int version)
int versions()
int node_count()
int root(int version)
int size(int version)
```

`fork` は同じ根を持つ新しい version を作る。

`versions`はversion数、`node_count`は使用node数、`root`はroot node番号、
`size`は重複込み要素数を返す。version範囲外は例外。

## count

```cpp
int count(int version, T value) const
bool contains(int version, T value) const
```

指定versionでの個数と存在有無を返す。どちらも$O(BIT)$。

## insert / erase

```cpp
int insert(int version, T x)
int erase(int version, T x)
```

新しい version を返す。`erase` は値が存在しない場合、同じ内容の新しい version を返す。

## 順序

```cpp
optional<T> kth(int version, int k, T xor_value = 0)
optional<T> min_element(int version, T xor_value = 0)
optional<T> max_element(int version, T xor_value = 0)
int count_less(int version, T x, T xor_value = 0)
```

`xor_value` を指定した場合、値 `y` は `y xor xor_value` の順で比較される。

## API別の時間計算量

$B=\mathrm{BIT}$ とする。

- `PersistentBinaryTrie()`: $O(\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- `versions`, `node_count`, `root`, `size`, `fork`: $O(1)$。`fork` はnodeを増やさずversionだけを生成する
- `count`, `contains`, `kth`, `min_element`, `max_element`, `count_less`: $O(B)$
- `insert`: $O(B)$、path-copy node $B+1$ 個と1 versionを生成する
- `erase`: $O(B)$。存在すればpath-copy node $B+1$ 個、存在しなければ0個。どちらも1 versionを生成する

## 空間計算量

- 固定node/version array: $O(\mathrm{MAX\_NODES}+\mathrm{MAX\_VERSIONS})$
- 各操作の追加領域: $O(1)$

## 注意点

- `T`は整数型、`0 < BIT <= T`のbit数。versionは`[0,versions())`。
- updateでnode/version容量を超える場合は例外で、既存versionとnode数を変えない。
- `kth`はkが範囲外なら`nullopt`、min/maxは空versionなら`nullopt`。
- erase対象が存在しない場合はnodeを生成せず、同じ内容のversionを1個作る。
- 順序queryは`value xor xor_value`で比較し、保存値を返す。
