---
title: Immutable B-tree Bit Sequence (不変B木ビット列)
documentation_of: ../../../../src/structure/wavelet_matrix/detail/immutable_btree_bit_sequence.hpp
---

更新のたびに不変な根（`Root`）を返す動的ビット列を、固定次数の B+ 木として管理する汎用の内部部品である。
葉はビット列と任意のペイロードを保持し、内部ノードは部分木の長さ・1 の個数・ビット別のペイロード和を保持する。
乱数による優先度や平方分割は使用しない。

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

- `Payload`：各ビットと一緒に保持する値の型。
- `Sum`：ペイロードの集約値の型。
- `Lift`：`Payload` を `Sum` へ変換する関数オブジェクト。
- `LEAF_WORDS`：1 つの葉に保持する 64 ビットワードの数。デフォルト値は 2。
- `INTERNAL_CAPACITY`：内部ノードの最大の子数。デフォルト値は 16。

`Payload` はデフォルト構築・コピー構築・コピー代入が可能でなければならない。
`Sum` はデフォルト構築・コピーが可能で、`+` と `-` を提供する必要がある。

ペイロードを持たない用途には次のエイリアスを使用する。

```cpp
wavelet_matrix_detail::ImmutableBTreeBitVector<> bits;
```

このエイリアスでは葉にペイロード配列を確保しない。

## 公開型

- `Root`：ある状態の B+ 木の根。コピーしても木本体は共有される。
- `Snapshot`：ノードプールの巻き戻し位置。
- `Entry`：`bit` と `payload` の組。
- `RankPair`：`[0, l)` と `[0, r)` の 1 の個数。
- `RangeStats`：接頭辞の 1 の個数と、`[l, r)` にある 0/1 それぞれのペイロード和。
- `EraseResult`：削除後の根と削除したエントリ。
- `PayloadUpdateResult`：更新後の根、ビット、更新前のペイロード。

## 構築・プール管理

```cpp
Root build(bits, payloads);
Root build(bits); // ImmutableBTreeBitVector
Snapshot snapshot() const;
void rollback(Snapshot snapshot);
std::size_t nodes_used() const;
```

- `build` は入力を順序どおりに保持する平衡 B+ 木を一括構築する。`bits` と `payloads` の長さが異なれば例外を送出する。
- `snapshot` は現在の葉・内部ノードのプールの長さを取得する。
- `rollback` はスナップショット以後に作られたノードを破棄する。破棄したノードを指す根は無効になる。
- 更新処理は例外発生時に、自身が追加したノードを自動で巻き戻す。複数の階層をまとめて更新する呼び出し側は、処理前のスナップショットへさらに巻き戻せる。

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

- 添字・出現番号は 0-indexed、区間は半開区間である。
- `select` は指定した出現がなければ `size(root)` を返す。
- 3 引数の `weight_of_first` は列全体の先頭から、4 引数版は位置 `left` 以後から、指定したビットを先頭から `count` 個選び、そのペイロード和を返す。`count` が該当個数を超えれば例外を送出する。
- ペイロードを持たないエイリアスでは `sum` と `weight_of_first` を提供しない。

## 不変更新

```cpp
Root insert(Root root, int position, bool bit, Payload payload);
EraseResult erase(Root root, int position);
PayloadUpdateResult set_payload(
    Root root, int position, Payload payload
);
```

更新は根から対象の葉までを経路コピーする。分割・借用・併合で変更する兄弟だけを追加でコピーし、入力の根以下は変更しない。
`set_payload` はビットと列長を変更しない。

## 時間計算量

$N$ を `size(root)`、葉の容量を $C = 64 \cdot \texttt{LEAF_WORDS}$、内部ノードの最大子数を $B$（`INTERNAL_CAPACITY`）、
木の高さを $H = O(\log_B(N/C+1))$ とする。

- `build`：$O(N)$
- `size`, `ones`, `snapshot`, `nodes_used`：$O(1)$
- `access`, `rank`, `rank_pair`, `select`：$O(H+C)$
- `range_stats`, `sum`, `sum_all`, `weight_of_first`：$O(H+C)$
- `insert`, `erase`：$O(H+C)$ 時間、$O(H)$ 個のノード
- `set_payload`：$O(H+C)$ 時間、$O(H)$ 個のノード
- `rollback`：破棄するノード数に比例

$C$ と $B$ はコンパイル時定数なので、各参照・更新は決定的に最悪 $O(\log N)$ である。
全ての根を含む空間計算量は、初期列と全更新で実際に経路コピーしたノード数に比例する。

## 注意点

- 根は、同じ `ImmutableBTreeBitSequence` オブジェクトのノードプールと組でのみ有効である。
- `rollback` で破棄された（巻き戻し位置より後に作られた）根を再利用してはならない。
- 根以外の葉は容量の半分以上、根以外の内部ノードは最大子数の半分以上を保つ。
- `insert` / `erase` は入力の根を変更しないため、既存の根に対するクエリ結果は更新後も変化しない。
- 標準 C++20 だけで実装され、GCC 13 で利用できる。
