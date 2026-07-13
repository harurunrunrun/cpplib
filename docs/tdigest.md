---
title: t-digest (tダイジェストによる分位点スケッチ)
documentation_of: ../src/approximate/streaming/tdigest.hpp
---

近い値を weighted centroid にまとめる分位点 sketch。中央より tail の centroid を
細かく残す scale を使う。乱数を使わず決定的。

- `add(value, weight)`: 有限値と正の有限重みを追加する。centroid 挿入と必要な 1 組の
  圧縮を行う。$O(MaxCentroids)$。
- `merge(other)`: snapshot した `other` の centroid を追加する。自己 merge も可能。
  $O(MaxCentroids^2)$。
- `quantile(probability)`: centroid 中心間を補間する。$O(MaxCentroids)$。
  空または `[0,1]` 外は例外。
- `cdf(value)`: 累積分布を補間する。NaN は `std::invalid_argument`。$O(MaxCentroids)$。
- `clear()`: $O(1)$。
- `centroid_count()`, `total_weight()`, `maximum_centroids()`: $O(1)$。

t-digest には入力分布に依存しない厳密な最大 rank 誤差保証はない。
centroid 数を増やすと通常は誤差が下がり、この実装は tail の結合を中央より抑える。
保存領域は $O(MaxCentroids)$。
