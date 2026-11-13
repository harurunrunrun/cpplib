---
title: Cuckoo Filter (カッコウフィルタ)
documentation_of: ../../../src/approximate/streaming/cuckoo_filter.hpp
---

各キーの短い fingerprint を 2 つの候補 bucket のどちらかに置く、容量固定の所属判定フィルタである。

## 使い方

~~~cpp
CuckooFilter<> filter(1024, 4, 500, 123); // bucket 数、slot 数、kick 上限、シード
bool inserted = filter.add(42);
bool present = filter.contains(42);

CuckooFilter<1024> fixed(123);            // 容量をテンプレート引数で固定する版
~~~

テンプレート引数は `CuckooFilter<BucketCount, SlotsPerBucket, FingerprintBits = 12, MaxKicks, Key = std::uint64_t>`。`BucketCount` を省略すると、bucket 数 $B$、各 bucket の slot 数 $S$（デフォルト 4）、kick 上限 $K$（デフォルト 500）を実行時に指定して `std::vector` に保存する可変長版になる。`BucketCount` を指定すると固定長版になり、`SlotsPerBucket` を省略すれば 4、`MaxKicks` を省略すれば 500 が使われる。

## API

- `CuckooFilter(bucket_count, slots_per_bucket = 4, maximum_kicks = 500, seed = 0)`（可変長版）/ `CuckooFilter(seed = 0)`（固定長版）: 空で構築する。
- `add(key)`: 候補 bucket に空き slot があれば置き、なければ最大 $K$ 回の追い出し（relocation）を行う。成功時は `true`、容量不足なら変更をすべて元に戻して `false` を返す。同じ fingerprint が候補 bucket に既にあれば追加済みとみなして何もせず `true` を返す。
- `contains(key)`: 2 つの候補 bucket を調べる。
- `erase(key)`: 一致する fingerprint を 1 個消し、消せたら `true` を返す。
- `clear()`: 空に戻す。
- `size()`, `capacity()`, `load_factor()`: 保存中の fingerprint 数、$BS$、その比を返す。

## 時間計算量

- コンストラクタ・`clear`: $O(BS)$、空間計算量 $O(BS)$
- `add`: 期待 $O(S)$、最悪 $O(KS)$、追加空間 $O(K)$
- `contains`, `erase`: $O(S)$
- `size`, `capacity`, `load_factor`: $O(1)$

## 注意点

- 偽陰性はない。ただし fingerprint の衝突があるため、`contains` と `erase` は偽陽性になり得る。低負荷時の偽陽性率は概ね $2S/2^{F}$（$F$ は `FingerprintBits`）である。
- 偽陽性のキーを `erase` すると別のキーの fingerprint を消して偽陰性を生むので、削除は実際に追加したキーに限る。
- 同じ fingerprint の重複は保存しないため、同じキーを複数回 `add` しても 1 回の `erase` で消える。
- $B$ は 2 以上の 2 の冪、$S$, $K$ は正、`FingerprintBits` は 1 以上 16 以下でなければならない。固定長版は `static_assert`、可変長版は `std::invalid_argument`（$BS$ のオーバーフローは `std::length_error`）で検査する。
