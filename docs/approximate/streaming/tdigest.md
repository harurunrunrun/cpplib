---
title: t-digest (tダイジェストによる分位点スケッチ)
documentation_of: ../../../src/approximate/streaming/tdigest.hpp
---

近い値を重み付きの重心（centroid）にまとめる分位点スケッチである。中央より裾（tail）の重心を細かく残すスケールを使う。乱数を使わず決定的に動作する。

## 使い方

~~~cpp
TDigest<> sketch(128);    // 重心数の上限
sketch.add(42.0L);
auto median = sketch.quantile(0.5L);

TDigest<128> fixed;       // 上限をテンプレート引数で固定する版
~~~

`TDigest<MaxCentroids>` の `MaxCentroids` を省略すると、重心数の上限 $C$（デフォルト 200）を実行時に指定して `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。

## API

- `TDigest::Centroid`: 重心の平均 `mean` と重み `weight` を保持する構造体。
- `TDigest(maximum_centroids = 200)`（可変長版）/ `TDigest()`（固定長版）: 空で構築する。
- `add(value, weight = 1.0L)`: 有限な値と正の有限な重みを追加する。重心を挿入し、上限を超えたら隣接する 1 組を結合する。
- `merge(other)`: `other` の重心のコピーを順に `add` する。自分自身との `merge` も可能。
- `quantile(probability)`: 重心の中心の間を線形補間して分位点を返す。
- `cdf(value)`: 累積分布関数の値を線形補間して返す。空なら 0。
- `clear()`: 空に戻す。
- `centroid_count()`, `total_weight()`, `maximum_centroids()`: 重心の個数、総重み、$C$ を返す。

## 時間計算量

- コンストラクタ: $O(C)$、空間計算量 $O(C)$
- `add`, `quantile`, `cdf`: $O(C)$、追加空間 $O(1)$
- `merge`: $O(C^2)$、追加空間 $O(C)$
- `clear`, `centroid_count`, `total_weight`, `maximum_centroids`: $O(1)$

## 注意点

- t-digest には、入力分布に依存しない厳密な最大順位誤差の保証はない。重心数を増やすと通常は誤差が下がる。
- `add` は値が非有限なら、重みが正の有限値でなければ `std::invalid_argument`、総重みが有限でなくなるなら `std::overflow_error` を投げる。
- 空のスケッチへの `quantile` は `std::logic_error`、`probability` が $[0,1]$ の外なら `std::out_of_range`、`cdf` に NaN を渡すと `std::invalid_argument` を投げる。
- $C$ は 2 以上でなければならない。固定長版は `static_assert`、可変長版は `std::invalid_argument` で検査する。
