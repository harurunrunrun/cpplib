---
title: KLL Sketch (KLLスケッチ)
documentation_of: ../src/approximate/streaming/kll_sketch.hpp
---

## KLL Sketch (KLLスケッチ)

各 level に最大 `K` 個を置き、sort 後の偶数番または奇数番をランダムに次 level へ
昇格する固定容量 quantile sketch。`K` は 2 以上の偶数。

## API

- `KLLSketch(seed, compare)`: 空で構築する。時間計算量は $O(K\,MaxLevels)$。 追加空間計算量は $O(1)$。
- `add(value)`: 値を追加し、満杯 level を圧縮する。償却時間計算量は $O(\log K)$、
  1 回の cascade は $O(K\,MaxLevels\log K)$。 追加空間計算量は $O(K\,MaxLevels)$。
- `quantile(probability)`: 保持値を重み付き sort し分位点を返す。
  保持数を $R$ として 時間計算量は $O(R\log R)$。空または範囲外は例外。 追加空間計算量は $O(R)$。
- `clear(seed)`: 時間計算量は $O(MaxLevels)$。 追加空間計算量は $O(1)$。
- `size()`, `retained_items()`: 時間計算量はそれぞれ $O(1)$, $O(MaxLevels)$。 追加空間計算量は $O(1)$。
- `capacity_per_level()`, `maximum_levels()`, `typical_rank_error_rate()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

独立な compaction parity の下で rank 誤差は典型的に $O(N/\sqrt K)$、失敗確率は
$e^{-\Theta(K\varepsilon^2)}$。最上位からさらに昇格が必要なら
`std::overflow_error`。保存に必要な空間計算量は $O(K\,MaxLevels)$。
