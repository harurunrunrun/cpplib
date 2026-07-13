---
title: Bloom Filter (ブルームフィルタ)
documentation_of: ../src/approximate/streaming/bloom_filter.hpp
---

# Bloom Filter (ブルームフィルタ)

集合への所属を固定長 bit 列で近似する `BloomFilter` と、削除・多重度推定に対応する
`CountingBloomFilter`。hash の seed は constructor で指定する。

## API

### `BloomFilter<BitCount, HashCount, Key>`

- `BloomFilter(seed)`: 指定seedで空filterを構築する。時間計算量は $O(BitCount/64)$、追加空間計算量は $O(1)$。
- `add(key)`: `key` を追加する。時間計算量は $O(HashCount)$。 追加空間計算量は $O(1)$。
- `contains(key)`: 所属候補なら `true`。追加済み要素には必ず `true` を返す。時間計算量は $O(HashCount)$。 追加空間計算量は $O(1)$。
- `clear()`: 空に戻す。時間計算量は $O(BitCount/64)$。 追加空間計算量は $O(1)$。
- `insertions()`: 追加回数。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `bit_count()`, `hash_count()`: template 容量。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `estimated_false_positive_probability()`: 現在の追加回数から偽陽性率
  $(1-e^{-kn/m})^k$ を返す。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


一様独立 hash の仮定で上式が偽陽性率の近似値となる。保存に必要な空間計算量は $O(BitCount)$ bit。

### `CountingBloomFilter<CounterCount, HashCount, Counter, Key>`

- `CountingBloomFilter(seed)`: 指定seedで空filterを構築する。時間計算量は $O(CounterCount)$、追加空間計算量は $O(1)$。
- `add(key)`: 対応 counter を飽和加算する。時間計算量は $O(HashCount)$。 追加空間計算量は $O(1)$。
- `remove(key)`: 全 counter が正なら減算して `true`、そうでなければ `false`。時間計算量は $O(HashCount)$。 追加空間計算量は $O(1)$。
- `contains(key)`: 全 counter が正かを返す。時間計算量は $O(HashCount)$。 追加空間計算量は $O(1)$。
- `estimate_multiplicity(key)`: 対応 counter の最小値を返す。真の多重度以上になり得る。時間計算量は $O(HashCount)$。 追加空間計算量は $O(1)$。
- `clear()`: 全 counter を 0 にする。時間計算量は $O(CounterCount)$。 追加空間計算量は $O(1)$。
- `insertions()`, `counter_count()`, `hash_count()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

削除は実際に追加した回数以下に限る。偽陽性となった未追加 key の削除は、保存中 key
と counter を共有して偽陰性を発生させ得る。counter 飽和後の削除も偽陰性を避けられない。
保存に必要な空間計算量は $O(CounterCount)$。
