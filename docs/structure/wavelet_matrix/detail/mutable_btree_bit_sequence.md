---
title: Mutable B-tree Bit Sequence (可変B木ビット列)
documentation_of: ../../../../src/structure/wavelet_matrix/detail/mutable_btree_bit_sequence.hpp
---

ビットと任意のペイロードの組の列を、決定的な平衡 B+ 木で管理する内部部品である。
葉は固定上限個の要素を、内部ノードは固定上限個の子を持ち、各ノードに
長さ、1 の個数、ペイロードの総和を保持する。乱数に依存せず、ノードの所有権は `std::unique_ptr` で管理する。再帰の深さが
列長に比例するような木構造も使わない。

通常は Dynamic Fully Indexable Dictionary や Dynamic Weighted Wavelet Matrix
から利用し、競技コードから直接利用する必要はない。

## テンプレート引数

```cpp
wavelet_matrix_detail::MutableBTreeBitSequence<
    Payload,
    Sum,
    Lift,
    LEAF_CAPACITY,
    INTERNAL_CAPACITY
>
```

`Payload` は各ビットに対応する値、`Sum` は集約値の型、`Lift` は
`Payload` を `Sum` へ変換する関数オブジェクトである。`LEAF_CAPACITY` と
`INTERNAL_CAPACITY` はそれぞれ葉の要素数の上限と内部ノードの子数の上限である。
ペイロードを持たない場合は `MutableBTreeNoPayload`、
`MutableBTreeNoSum`、`MutableBTreeNoPayloadLift` を指定する。

## 構築・コピー・取得

```cpp
MutableBTreeBitSequence()
MutableBTreeBitSequence(const MutableBTreeBitSequence&)
MutableBTreeBitSequence(MutableBTreeBitSequence&&)
MutableBTreeBitSequence& operator=(const MutableBTreeBitSequence&)
MutableBTreeBitSequence& operator=(MutableBTreeBitSequence&&)
void assign(const vector<unsigned char>& bits, const vector<Payload>& payloads = {})
int size() const
int ones() const
Entry access(int position) const
Stats prefix_stats(int end) const
Stats range_stats(int left, int right) const
```

`assign` は列全体を置き換える。コピーはノードとペイロードを複製し、ムーブは木の所有権を移す。
`Stats` は区間長、1 の個数、全ペイロード和、0/1 ごとのペイロード和を持つ。

## `rank`・`select`・集約

```cpp
int rank(bool bit, int end) const
int select(bool bit, int occurrence) const
Sum sum_first(bool bit, int count) const
Sum sum_first(bool bit, int left, int count) const
```

`select` は指定した出現がなければ `size()` を返す。
2 引数の `sum_first` は列の先頭から、3 引数の `sum_first` は位置 `left` 以降に現れる指定ビットのうち、
先頭 `count` 個に対応するペイロードの和を返す。

## 更新

```cpp
void insert(int position, bool bit, const Payload& payload = {})
Entry erase(int position)
bool set_bit(int position, bool bit)
bool flip(int position)
bool set_payload(int position, const Payload& payload)
```

`erase` は削除したエントリを、`set_bit` と `set_payload` は更新前のビットを、
`flip` は反転前のビットを返す。分割・借用・併合により B+ 木の
占有率を保つ。

## 時間計算量

現在の列長を $N$、木の高さを
$H=O(\log_{B}(N/C+1))$ とする。ここで $B$ は `INTERNAL_CAPACITY`、$C$ は `LEAF_CAPACITY` である。
テンプレートで指定するノード容量は定数である。

- `assign`: $O(N)$
- `size`, `ones`: $O(1)$
- `access`, `rank`, `select`, `prefix_stats`, `range_stats`,
  `sum_first`: 最悪 $O(H)$
- `insert`, `erase`, `set_bit`, `flip`, `set_payload`:
  最悪 $O(H)$
- コピー構築・コピー代入: $O(N)$
- ムーブ構築・ムーブ代入: $O(1)$
- 空間計算量: $O(N)$

## 注意点

- `Payload` と `Sum` はデフォルト構築とコピーが可能でなければならない。
- `Sum` は加算と減算をサポートしなければならない。
- 添字の検査は、利用側の公開データ構造が行う内部契約である。
- 標準 C++20 と GCC 13 で利用できる。
