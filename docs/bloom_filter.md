---
title: Bloom Filter (ブルームフィルタ)
documentation_of: ../src/approximate/streaming/bloom_filter.hpp
---

集合への所属を固定長 bit 列で近似する `BloomFilter` と、削除・多重度推定に対応する
`CountingBloomFilter`。hash の seed は constructor で指定する。

## `BloomFilter<BitCount, HashCount, Key>`

- `add(key)`: `key` を追加する。$O(HashCount)$。
- `contains(key)`: 所属候補なら `true`。追加済み要素には必ず `true` を返す。$O(HashCount)$。
- `clear()`: 空に戻す。$O(BitCount/64)$。
- `insertions()`: 追加回数。$O(1)$。
- `bit_count()`, `hash_count()`: template 容量。$O(1)$。
- `estimated_false_positive_probability()`: 現在の追加回数から偽陽性率
  $(1-e^{-kn/m})^k$ を返す。$O(1)$。

一様独立 hash の仮定で上式が偽陽性率の近似値となる。保存領域は $O(BitCount)$ bit。

## `CountingBloomFilter<CounterCount, HashCount, Counter, Key>`

- `add(key)`: 対応 counter を飽和加算する。$O(HashCount)$。
- `remove(key)`: 全 counter が正なら減算して `true`、そうでなければ `false`。$O(HashCount)$。
- `contains(key)`: 全 counter が正かを返す。$O(HashCount)$。
- `estimate_multiplicity(key)`: 対応 counter の最小値を返す。真の多重度以上になり得る。$O(HashCount)$。
- `clear()`: 全 counter を 0 にする。$O(CounterCount)$。
- `insertions()`, `counter_count()`, `hash_count()`: $O(1)$。

削除は実際に追加した回数以下に限る。偽陽性となった未追加 key の削除は、保存中 key
と counter を共有して偽陰性を発生させ得る。counter 飽和後の削除も偽陰性を避けられない。
保存領域は $O(CounterCount)$。
