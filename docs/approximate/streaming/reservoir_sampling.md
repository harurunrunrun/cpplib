---
title: Reservoir Sampling (リザーバサンプリング)
documentation_of: ../../../src/approximate/streaming/reservoir_sampling.hpp
---

## Reservoir Sampling (リザーバサンプリング)

長さ不明の stream から固定個数を抽出する。

## API

### `ReservoirSampler<T, Capacity>`

- `ReservoirSampler(seed)`: 乱数 seed を指定する。時間計算量は $O(Capacity)$。 追加空間計算量は $O(1)$。
- `add(value)`: Algorithm R で追加する。期待時間計算量は $O(1)$。処理数が `uint64_t` の上限なら
  `std::overflow_error`。 追加空間計算量は $O(1)$。
- `samples()`: 現在の標本を `vector` で返す。時間計算量は $O(Capacity)$。 追加空間計算量は $O(Capacity)$（返値を含む）。
- `clear(seed)`: 空にして乱数を再初期化する。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `size()`, `seen()`, `capacity()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


$n\ge Capacity$ のとき、各要素が標本に含まれる確率は厳密に $Capacity/n$。
保存に必要な空間計算量は $O(Capacity)$。

### `WeightedReservoirSampler<T, Capacity>`

- `WeightedReservoirSampler(seed)`: 指定seedで空sampleを構築する。時間計算量は $O(Capacity)$、追加空間計算量は $O(1)$。
- `add(value, weight)`: 正の有限重みから priority $\log U/weight$ を作り、上位を保持する。
  時間計算量は $O(\log Capacity)$。非正または非有限の重みは `std::invalid_argument`。 追加空間計算量は $O(1)$。
- `samples()`: 順序を保証しない標本を返す。時間計算量は $O(Capacity)$。 追加空間計算量は $O(Capacity)$（返値を含む）。
- `clear(seed)`, `size()`, `seen()`, `capacity()`: 時間計算量・追加空間計算量はいずれも $O(1)$。


## 注意点

指数 race による重複なしの重み付き抽出であり、次に選ばれる要素の条件付き確率は
残存重みに比例する。保存に必要な空間計算量は $O(Capacity)$。

## 容量を省略する可変長版

~~~cpp
ReservoirSampler<int> uniform(32, 123); // 標本数、seed
WeightedReservoirSampler<int> weighted(32, 123);
uniform.add(42);
weighted.add(42, 2.0L);
auto samples = uniform.samples();
~~~

標本数 $C$ を実行時に指定し、`vector` に保存する。構築は $O(C)$ 時間・保存領域 $O(C)$。
通常版の追加は期待 $O(1)$、重み付き版は $O(\log C)$ 時間で、追加の一時領域は $O(1)$。
標本取得は $O(C)$ 時間・返り値 $O(C)$ 空間、`clear` は $O(1)$ 時間。
容量を明示した固定配列版も同じ header にある。
