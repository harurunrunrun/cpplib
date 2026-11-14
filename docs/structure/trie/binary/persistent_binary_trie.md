---
title: Persistent Binary Trie (完全永続二進トライ)
documentation_of: ../../../../src/structure/trie/binary/persistent_binary_trie.hpp
---

完全永続の Binary Trie である。整数の多重集合を版（バージョン）ごとに管理し、同じ値を重複して保持できる。構築直後は空の版 0 だけが存在する。

## テンプレート引数

```cpp
PersistentBinaryTrie<T, BIT, MAX_NODES, MAX_VERSIONS>
```

`BIT` は `T` のビット幅以下とする（デフォルトは `T` に対応する符号なし型のビット幅）。`MAX_NODES` と `MAX_VERSIONS` は省略でき、デフォルト値 `-1` では可変長版になる。更新が容量不足で失敗した場合、版とノードは消費されない。

## 版

```cpp
int fork(int version)
int versions()
int node_count()
int root(int version)
int size(int version)
```

`fork` は `version` と同じ根を持つ新しい版を作り、その番号を返す。

`versions` は版の数、`node_count` は使用ノード数、`root` は根ノードの番号、
`size` は重複を含む要素数を返す。版の番号が範囲外なら `runtime_error` を送出する。

## 個数

```cpp
int count(int version, T value) const
bool contains(int version, T value) const
```

指定した版での個数と、存在するかどうかを返す。

## `insert` / `erase`

```cpp
int insert(int version, T x)
int erase(int version, T x)
```

`version` に `x` を 1 個追加 / 削除した新しい版の番号を返す。`erase` は値が存在しない場合、同じ内容の新しい版を返す。

## 順序

```cpp
optional<T> kth(int version, int k, T xor_value = 0)
optional<T> min_element(int version, T xor_value = 0)
optional<T> max_element(int version, T xor_value = 0)
int count_less(int version, T x, T xor_value = 0)
```

`xor_value` を指定した場合、値 `y` は `y ^ xor_value` の大小で順序付けられる。

## API 別の時間計算量

$B$ を `BIT` とする。

- `PersistentBinaryTrie()`: 固定長版は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$、可変長版は $O(1)$
- `versions`, `node_count`, `root`, `size`, `fork`: $O(1)$。`fork` はノードを増やさず版だけを生成する
- `count`, `contains`, `kth`, `min_element`, `max_element`, `count_less`: $O(B)$
- `insert`: $O(B)$。経路コピーでノード $B+1$ 個と版 1 個を生成する
- `erase`: $O(B)$。値が存在すれば経路コピーでノード $B+1$ 個、存在しなければ 0 個を生成する。どちらも版を 1 個生成する

## 空間計算量

- ノード・版の配列: 固定長版は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS})$、可変長版は使用量に比例
- 各操作の追加領域: $O(1)$

## 注意点

- `T` は整数型で、`0 < BIT <=` （`T` のビット幅）でなければならない。版の番号は `[0, versions())` である。
- 更新でノード・版の容量を超える場合は `runtime_error` を送出し、既存の版とノード数は変えない。
- `kth` は `k` が範囲外なら `nullopt` を、`min_element` / `max_element` は空の版なら `nullopt` を返す。
- `erase` の対象が存在しない場合はノードを生成せず、同じ内容の版を 1 個作る。
- 順序に関するクエリは `value ^ xor_value` で比較し、保存値を返す。
- 容量を指定した固定配列版（`PersistentBinaryTrie<T, BIT, NODES, VERSIONS>`）と可変長版（`PersistentBinaryTrie<T>`）は同じヘッダで利用できる。可変長版は必要に応じて領域を伸長し、その費用はならして上記の計算量に含まれる。
