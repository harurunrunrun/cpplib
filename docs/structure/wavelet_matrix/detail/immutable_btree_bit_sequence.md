---
title: Immutable B-tree Bit Sequence (不変B木ビット列)
documentation_of: ../../../../src/structure/wavelet_matrix/detail/immutable_btree_bit_sequence.hpp
---

不変rootを返す動的ビット列を、固定次数のB+木として管理する中立な内部部品。
葉はビット列と任意のpayloadを保持し、内部nodeは部分木長・1の個数・bit別payload和を保持する。
乱数優先度や平方分割は使用しない。

通常は Wavelet Matrix の実装から利用し、競技コードから直接利用する必要はない。

## テンプレート引数

```cpp
wavelet_matrix_detail::ImmutableBTreeBitSequence<
    Payload,
    Sum,
    Lift,
    LEAF_WORDS,
    INTERNAL_CAPACITY
>
```

- `Payload`：各bitと一緒に保持する値の型。
- `Sum`：payload集約値の型。
- `Lift`：`Payload`を`Sum`へ変換する関数オブジェクト。
- `LEAF_WORDS`：1葉に保持する64bit word数。既定値は2。
- `INTERNAL_CAPACITY`：内部nodeの最大子数。既定値は16。

`Payload`はdefault構築・copy構築・copy代入が可能でなければならない。
`Sum`はdefault構築・copyが可能で、`+`と`-`を提供する必要がある。

payloadを持たない用途には次のaliasを使用する。

```cpp
wavelet_matrix_detail::ImmutableBTreeBitVector<> bits;
```

このaliasでは葉にpayload配列を確保しない。

## 公開型

- `Root`：ある状態のB+木root。copyしても木本体は共有される。
- `Snapshot`：node poolの巻き戻し位置。
- `Entry`：`bit`と`payload`。
- `RankPair`：`[0,l)`と`[0,r)`の1の個数。
- `RangeStats`：prefixの1の個数と、`[l,r)`にある0/1それぞれのpayload和。
- `EraseResult`：削除後rootと削除したentry。
- `PayloadUpdateResult`：更新後root、bit、更新前payload。

## 構築・pool管理

```cpp
Root build(bits, payloads);
Root build(bits); // ImmutableBTreeBitVector
Snapshot snapshot() const;
void rollback(Snapshot snapshot);
std::size_t nodes_used() const;
```

- `build`は入力を順序どおり保持する平衡B+木をbulk buildする。
- `snapshot`は現在のleaf/internal pool長を取得する。
- `rollback`はsnapshot以後に作られたnodeを破棄する。破棄したnodeを指すrootは無効になる。
- 更新処理は例外時に自身が追加したnodeを自動で巻き戻す。複数階層をまとめて更新する呼出側は、処理前snapshotへさらに巻き戻せる。

## 参照

```cpp
int size(Root root) const;
int ones(Root root) const;
Entry access(Root root, int position) const;
int rank(Root root, bool bit, int end) const;
int rank(Root root, bool bit, int left, int right) const;
RankPair rank_pair(Root root, int left, int right) const;
int select(Root root, bool bit, int occurrence) const;
RangeStats range_stats(Root root, int left, int right) const;
Sum sum(Root root, bool bit, int left, int right) const;
Sum sum_all(Root root, int left, int right) const;
Sum weight_of_first(Root root, bool bit, int count) const;
Sum weight_of_first(
    Root root, bool bit, int left, int count
) const;
```

- 添字・出現番号は0-indexed、区間は半開区間。
- `select`は指定出現がなければ`size(root)`を返す。
- 4引数の`weight_of_first`は列全体で先頭から`count`個、5引数版は位置`left`以後にある指定bitを先頭から`count`個選び、そのpayload和を返す。
- payloadを持たないaliasでは`sum`と`weight_of_first`を提供しない。

## 不変更新

```cpp
Root insert(Root root, int position, bool bit, Payload payload);
EraseResult erase(Root root, int position);
PayloadUpdateResult set_payload(
    Root root, int position, Payload payload
);
```

更新はrootから対象葉までをpath copyする。split・borrow・mergeで変更する兄弟だけを追加copyし、入力root以下は変更しない。
`set_payload`はbitと列長を変更しない。

## 時間計算量

`N = size(root)`、葉容量を`C = 64 * LEAF_WORDS`、内部最大子数を`B = INTERNAL_CAPACITY`、
木の高さを`H = O(log_B(N/C+1))`とする。

- `build`：$O(N)$
- `size`, `ones`, `snapshot`, `nodes_used`：$O(1)$
- `access`, `rank`, `rank_pair`, `select`：$O(H+C)$
- `range_stats`, `sum`, `sum_all`, `weight_of_first`：$O(H+C)$
- `insert`, `erase`：$O(H+C)$ 時間、$O(H)$ node
- `set_payload`：$O(H+C)$ 時間、$O(H)$ node
- `rollback`：破棄node数に比例

`C`と`B`はcompile時定数なので、各参照・更新は決定的worst-case $O(\log N)$。
全rootを含む空間計算量は、初期列と全更新で実際にpath copyしたnode数に比例する。

## 注意点

- rootは同じ`ImmutableBTreeBitSequence` objectのnode poolと組でのみ有効。
- `rollback`より後に作られたrootを再利用してはならない。
- root以外の葉は半分以上、root以外の内部nodeは最大子数の半分以上を保つ。
- `insert` / `erase`は入力rootを変更しないため、既存rootへのquery結果は更新後も変化しない。
- 標準C++20だけで実装され、GCC 13で利用できる。
