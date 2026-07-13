---
title: KLL Sketch (KLLスケッチ)
documentation_of: ../src/approximate/streaming/kll_sketch.hpp
---

各 level に最大 `K` 個を置き、sort 後の偶数番または奇数番をランダムに次 level へ
昇格する固定容量 quantile sketch。`K` は 2 以上の偶数。

- `KLLSketch(seed, compare)`: 空で構築する。$O(K\,MaxLevels)$。
- `add(value)`: 値を追加し、満杯 level を圧縮する。償却 $O(\log K)$、
  1 回の cascade は $O(K\,MaxLevels\log K)$。
- `quantile(probability)`: 保持値を重み付き sort し分位点を返す。
  保持数を $R$ として $O(R\log R)$。空または範囲外は例外。
- `clear(seed)`: $O(MaxLevels)$。
- `size()`, `retained_items()`: それぞれ $O(1)$, $O(MaxLevels)$。
- `capacity_per_level()`, `maximum_levels()`, `typical_rank_error_rate()`: $O(1)$。

独立な compaction parity の下で rank 誤差は典型的に $O(N/\sqrt K)$、失敗確率は
$e^{-\Theta(K\varepsilon^2)}$。最上位からさらに昇格が必要なら
`std::overflow_error`。保存領域は $O(K\,MaxLevels)$。
