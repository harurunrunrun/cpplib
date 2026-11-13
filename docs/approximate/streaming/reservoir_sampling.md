---
title: Reservoir Sampling (リザーバサンプリング)
documentation_of: ../../../src/approximate/streaming/reservoir_sampling.hpp
---

長さが事前に分からないストリームから、固定個数 $C$ の標本を抽出する。

## 使い方

~~~cpp
ReservoirSampler<int> uniform(32, 123);           // 標本数、シード
WeightedReservoirSampler<int> weighted(32, 123);
uniform.add(42);
weighted.add(42, 2.0L);
auto samples = uniform.samples();

ReservoirSampler<int, 32> fixed(123);             // 標本数をテンプレート引数で固定する版
~~~

`ReservoirSampler<T, Capacity>`、`WeightedReservoirSampler<T, Capacity>` の `Capacity` を省略すると、標本数 $C$ を実行時に指定して `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。

## API

### `ReservoirSampler`

- `ReservoirSampler(capacity, seed = 0)`（可変長版）/ `ReservoirSampler(seed = 0)`（固定長版）: 乱数のシードを指定して空で構築する。
- `add(value)`: Algorithm R で値を追加する。
- `samples()`: 現在の標本を `std::vector<T>` で返す。
- `clear(seed = 0)`: 空にして乱数生成器を初期化し直す。
- `size()`, `seen()`, `capacity()`: 現在の標本数、処理した要素数、$C$ を返す。

処理した要素数 $n$ が $C$ 以上のとき、各要素が標本に含まれる確率は厳密に $C/n$ である。

### `WeightedReservoirSampler`

- `WeightedReservoirSampler(capacity, seed = 0)`（可変長版）/ `WeightedReservoirSampler(seed = 0)`（固定長版）: 空で構築する。
- `add(value, weight)`: 正の有限な重み $w$ と一様乱数 $u\in(0,1]$ から優先度 $\log u/w$ を作り、優先度が上位 $C$ 個の要素を最小ヒープで保持する。
- `samples()`: 標本を `std::vector<T>` で返す。順序は保証しない。
- `clear(seed = 0)`, `size()`, `seen()`, `capacity()`: `ReservoirSampler` と同様。

指数レースによる重複なしの重み付き抽出であり、次に選ばれる要素の条件付き確率は残っている要素の重みに比例する。

## 時間計算量

- コンストラクタ: $O(C)$、空間計算量 $O(C)$
- `ReservoirSampler::add`: $O(1)$
- `WeightedReservoirSampler::add`: $O(\log C)$
- `samples`: $O(C)$（戻り値を含め追加空間 $O(C)$）
- `clear`, `size`, `seen`, `capacity`: $O(1)$

## 注意点

- 処理数が `std::uint64_t` の最大値に達した状態で `add` すると `std::overflow_error` を投げる。
- `WeightedReservoirSampler::add` は重みが正の有限値でなければ `std::invalid_argument` を投げる。
- 可変長版で $C=0$ なら `std::invalid_argument` を投げる。
