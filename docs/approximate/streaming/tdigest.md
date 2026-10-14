---
title: t-digest (tダイジェストによる分位点スケッチ)
documentation_of: ../../../src/approximate/streaming/tdigest.hpp
---

## t-digest (tダイジェストによる分位点スケッチ)

近い値を weighted centroid にまとめる分位点 sketch。中央より tail の centroid を
細かく残す scale を使う。乱数を使わず決定的。

## API

- `TDigest::Centroid`: centroidの平均 `mean` と重み `weight` を保持する。構築・各member参照の時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `TDigest()`: centroid数と総重みを0にして構築する。時間計算量と追加空間計算量はいずれも $O(MaxCentroids)$。
- `add(value, weight)`: 有限値と正の有限重みを追加する。centroid 挿入と必要な 1 組の
  圧縮を行う。時間計算量は $O(MaxCentroids)$。 追加空間計算量は $O(1)$。
- `merge(other)`: snapshot した `other` の centroid を追加する。自己 merge も可能。
  時間計算量は $O(MaxCentroids^2)$。 追加空間計算量は $O(MaxCentroids)$。
- `quantile(probability)`: centroid 中心間を補間する。時間計算量は $O(MaxCentroids)$。
  空または `[0,1]` 外は例外。 追加空間計算量は $O(1)$。
- `cdf(value)`: 累積分布を補間する。NaN は `std::invalid_argument`。時間計算量は $O(MaxCentroids)$。 追加空間計算量は $O(1)$。
- `clear()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `centroid_count()`, `total_weight()`, `maximum_centroids()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

t-digest には入力分布に依存しない厳密な最大 rank 誤差保証はない。
centroid 数を増やすと通常は誤差が下がり、この実装は tail の結合を中央より抑える。
保存に必要な空間計算量は $O(MaxCentroids)$。

## 容量を省略する可変長版

~~~cpp
TDigest<> sketch(128); // centroid 数の上限
sketch.add(42.0L);
auto median = sketch.quantile(0.5L);
~~~

centroid 上限 $C\ge2$ を実行時に指定し、`vector` に保存する。
構築・追加・分位点 query・CDF は $O(C)$ 時間、merge は $O(C^2)$ 時間。
保存領域は $O(C)$、merge の一時領域は $O(C)$、その他の一時領域は $O(1)$。
`clear` は $O(1)$ 時間。容量を明示した固定配列版も同じ header にある。
