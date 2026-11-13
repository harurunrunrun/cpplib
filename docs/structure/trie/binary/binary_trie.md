---
title: Binary Trie (二進トライ) [set_xor_min]
documentation_of: ../../../../src/structure/trie/binary/binary_trie.hpp
---

整数の多重集合を管理する Binary Trie である。同じ値を重複して保持できる。

## テンプレート引数

```cpp
BinaryTrie<T, BIT, MAX_NODES>
```

- 整数型 `T`
- ビット数 `BIT`（デフォルトは `T` に対応する符号なし型のビット幅）
- ノード数の上限 `MAX_NODES`（省略するとデフォルト値 `-1` の可変長版）

## `insert` / `erase`

```cpp
void insert(T x)
bool erase(T x)
```

`insert` は `x` を 1 個追加する。`erase` は `x` を 1 個削除して `true` を返し、値が存在しない場合は `false` を返す。

## 個数

```cpp
int count(T x)
bool contains(T x)
int size()
int node_count()
bool empty()
```

`count` は `x` の個数、`contains` は存在するか、`size` は重複を含む要素数、`node_count` は使用ノード数、`empty` は空かを返す。

## 順序

```cpp
optional<T> kth(int k, T xor_value = 0)
optional<T> min_element(T xor_value = 0)
optional<T> max_element(T xor_value = 0)
int count_less(T x, T xor_value = 0)
```

`kth` は 0-indexed で `k` 番目に小さい値、`count_less` は `x` 未満の値の個数を返す。
`xor_value` を指定した場合、値 `y` は `y ^ xor_value` の大小で順序付けられる。

## API 別の時間計算量

$B$ を `BIT` とする。

- `BinaryTrie()`: 固定長版は $O(\texttt{MAX_NODES})$、可変長版は $O(1)$
- `size`, `node_count`, `empty`: $O(1)$
- `insert`, `count`, `contains`, `erase`: $O(B)$。`insert` で新しく作るノードは高々 $B$ 個
- `kth`, `min_element`, `max_element`, `count_less`: $O(B)$

## 空間計算量

- ノード配列: 固定長版は $O(\texttt{MAX_NODES})$、可変長版は使用ノード数に比例
- 各操作の追加領域: $O(1)$

## 注意点

- `T` は整数型、`BIT > 0`、`MAX_NODES > 0`（固定長版）でなければならない。
- `insert` で必要なノード数が `MAX_NODES` を超える場合は `runtime_error` を送出する。`erase` 後もノードは回収しない。
- `kth` は `0 <= k < size()` なら値を、それ以外は `nullopt` を返す。
- `min_element`, `max_element` は空なら `nullopt` を返す。
- 順序に関するクエリは保存値そのものではなく `value ^ xor_value` を比較するが、戻り値は保存値である。
- 容量を指定した固定配列版（`BinaryTrie<T, BIT, N>`）と可変長版（`BinaryTrie<T>`）は同じヘッダで利用できる。可変長版は必要に応じて領域を伸長し、その費用はならして上記の計算量に含まれる。
