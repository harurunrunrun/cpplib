---
title: Reservoir Sampling (リザーバサンプリング)
documentation_of: ../src/approximate/streaming/reservoir_sampling.hpp
---

長さ不明の stream から固定個数を抽出する。

## `ReservoirSampler<T, Capacity>`

- `ReservoirSampler(seed)`: 乱数 seed を指定する。$O(Capacity)$。
- `add(value)`: Algorithm R で追加する。期待 $O(1)$。処理数が `uint64_t` の上限なら
  `std::overflow_error`。
- `samples()`: 現在の標本を `vector` で返す。$O(Capacity)$。
- `clear(seed)`: 空にして乱数を再初期化する。$O(1)$。
- `size()`, `seen()`, `capacity()`: $O(1)$。

$n\ge Capacity$ のとき、各要素が標本に含まれる確率は厳密に $Capacity/n$。
保存領域は $O(Capacity)$。

## `WeightedReservoirSampler<T, Capacity>`

- `add(value, weight)`: 正の有限重みから priority $\log U/weight$ を作り、上位を保持する。
  $O(\log Capacity)$。非正または非有限の重みは `std::invalid_argument`。
- `samples()`: 順序を保証しない標本を返す。$O(Capacity)$。
- `clear(seed)`, `size()`, `seen()`, `capacity()`: 上と同じ。

指数 race による重複なしの重み付き抽出であり、次に選ばれる要素の条件付き確率は
残存重みに比例する。保存領域は $O(Capacity)$。
