---
title: Bloom Filter (ブルームフィルタ)
documentation_of: ../../../src/approximate/streaming/bloom_filter.hpp
---

集合への所属を固定長のビット列で近似的に判定する `BloomFilter` と、削除・多重度推定に対応する `CountingBloomFilter` を提供する。ハッシュのシードはコンストラクタで指定する。キーは `std::hash<Key>` とシードから 64 bit ハッシュを作り、ダブルハッシングで `HashCount` 個の位置を得る。

## 使い方

~~~cpp
BloomFilter<> filter(4096, 5, 123);           // ビット数、ハッシュ数、シード
CountingBloomFilter<> counting(1024, 5, 123); // カウンタ数、ハッシュ数、シード
filter.add(42);
counting.add(42);

BloomFilter<4096, 5> fixed(123);              // 容量をテンプレート引数で固定する版
~~~

テンプレート引数 `BloomFilter<BitCount, HashCount, Key = std::uint64_t>`、`CountingBloomFilter<CounterCount, HashCount, Counter = std::uint16_t, Key = std::uint64_t>` の容量を省略すると、容量を実行時に指定して `std::vector` に保存する可変長版になる。容量を指定すると `std::array` に保存する固定長版になる。

## API

### `BloomFilter`

- `BloomFilter(bit_count, hash_count, seed = 0)`（可変長版）/ `BloomFilter(seed = 0)`（固定長版）: 空のフィルタを構築する。
- `add(key)`: `key` を追加する。
- `contains(key)`: 所属の可能性があれば `true` を返す。追加済みの要素には必ず `true` を返す（偽陰性なし）。
- `clear()`: 空に戻す。
- `insertions()`: `add` の呼び出し回数を返す。
- `bit_count()`, `hash_count()`: ビット数 $m$・ハッシュ数 $k$ を返す。
- `estimated_false_positive_probability()`: 現在の追加回数 $n$ から偽陽性率の近似値 $(1-e^{-kn/m})^k$ を返す。ハッシュが一様独立という仮定の下での近似である。

### `CountingBloomFilter`

- `CountingBloomFilter(counter_count, hash_count, seed = 0)`（可変長版）/ `CountingBloomFilter(seed = 0)`（固定長版）: 空のフィルタを構築する。
- `add(key)`: 対応するカウンタを飽和加算する。
- `remove(key)`: 対応するカウンタがすべて正なら 1 ずつ減らして `true`、そうでなければ何もせず `false` を返す。
- `contains(key)`: 対応するカウンタがすべて正かを返す。
- `estimate_multiplicity(key)`: 対応するカウンタの最小値を返す。真の多重度以上になり得る。
- `clear()`: 全カウンタを 0 にする。
- `insertions()`, `counter_count()`, `hash_count()`: 追加回数（`remove` 成功で 1 減る）、カウンタ数、ハッシュ数を返す。

## 時間計算量

ビット数を $B$、カウンタ数を $C$、ハッシュ数を $H$ とする。

- `BloomFilter` のコンストラクタ・`clear`: $O(\lceil B/64\rceil)$、空間計算量 $O(\lceil B/64\rceil)$ 語
- `CountingBloomFilter` のコンストラクタ・`clear`: $O(C)$、空間計算量 $O(C)$
- `add`, `contains`, `remove`, `estimate_multiplicity`: $O(H)$、追加空間 $O(1)$
- その他のアクセサ: $O(1)$

## 注意点

- 削除は実際に追加した回数以下に限る。偽陽性となった未追加のキーを削除すると、保存中のキーとカウンタを共有しているため偽陰性が起こり得る。カウンタが飽和した後の削除も偽陰性を避けられない。
- `Counter` は符号なし整数型でなければならない（`static_assert`）。
- 可変長版で容量またはハッシュ数が 0 なら `std::invalid_argument`、表の大きさが `std::size_t` で表せなければ `std::length_error` を投げる。
