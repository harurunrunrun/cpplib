---
title: Mutable B-tree Bit Sequence (可変B木ビット列)
documentation_of: ../../../../src/structure/wavelet_matrix/detail/mutable_btree_bit_sequence.hpp
---

bit と任意の payload の列を、決定的な平衡 B+ 木で管理する内部部品。
葉は固定上限個の要素、内部 node は固定上限個の子を持ち、各 node に
長さ、1 の個数、payload の総和を保持する。乱数に依存せず、node 所有権は unique_ptr で管理する。再帰深さが
列長に比例する木構造も使わない。

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

`Payload` は各 bit に対応する値、`Sum` は集約値、`Lift` は
`Payload` を `Sum` へ変換する関数 object。`LEAF_CAPACITY` と
`INTERNAL_CAPACITY` はそれぞれ葉の要素数上限と内部 node の子数上限。
payload を持たない場合は `MutableBTreeNoPayload`、
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

コピーは node と payload を複製する。ムーブは木の所有権を移す。
`Stats` は区間長、1 の個数、全 payload 和、0/1 ごとの payload 和を持つ。

## rank・select・集約

```cpp
int rank(bool bit, int end) const
int select(bool bit, int occurrence) const
Sum sum_first(bool bit, int count) const
Sum sum_first(bool bit, int left, int count) const
```

`select` は指定した出現がなければ `size()` を返す。
3 引数の `sum_first` は位置 `left` 以降に現れる指定 bit のうち、
先頭 `count` 個に対応する payload の和を返す。

## 更新

```cpp
void insert(int position, bool bit, const Payload& payload = {})
Entry erase(int position)
bool set_bit(int position, bool bit)
bool flip(int position)
bool set_payload(int position, const Payload& payload)
```

`erase` は削除した entry、`set_bit` と `set_payload` は更新前の bit、
`flip` は反転前の bit を返す。split、borrow、merge により B+ 木の
占有率を保つ。

## 時間計算量

現在の列長を $N$、木の高さを
$H=O(\log_{INTERNAL\_CAPACITY}(N/LEAF\_CAPACITY+1))$ とする。
テンプレートで指定する node 容量は定数である。

- `assign`: $O(N)$
- `size`, `ones`: $O(1)$
- `access`, `rank`, `select`, `prefix_stats`, `range_stats`,
  `sum_first`: worst-case $O(H)$
- `insert`, `erase`, `set_bit`, `flip`, `set_payload`:
  worst-case $O(H)$
- コピー構築・コピー代入: $O(N)$
- ムーブ構築・ムーブ代入: $O(1)$
- 空間計算量: $O(N)$

## 注意点

- `Payload` と `Sum` は default 構築とコピーが可能でなければならない。
- `Sum` は加算と減算をサポートしなければならない。
- 添字検査は利用側の公開 data structure が行う内部契約である。
- 標準 C++20 と GCC 13 で利用できる。
