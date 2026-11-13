---
title: KLL Sketch (KLLスケッチ)
documentation_of: ../../../src/approximate/streaming/kll_sketch.hpp
---

各レベルに最大 $K$ 個の値を置き、満杯になったレベルをソートして偶数番目または奇数番目をランダムに選んで次のレベルへ昇格させる、容量固定の分位点スケッチである。レベル $\ell$ の値は重み $2^\ell$ を持つ。

## 使い方

~~~cpp
KLLSketch<int> sketch(128, 24, 123); // レベルごとの容量、レベル数の上限、シード
sketch.add(42);
auto median = sketch.quantile(0.5L);

KLLSketch<int, 128> fixed(123);      // 容量をテンプレート引数で固定する版（レベル数 32）
~~~

`KLLSketch<T, K, MaxLevels, Compare = std::less<T>>` の `K` を省略すると、容量 $K$（デフォルト 200）とレベル数の上限 $L$（デフォルト 32）を実行時に指定して `std::vector` に保存する可変長版になる。`K` を指定すると `std::array` に保存する固定長版になり、`MaxLevels` を省略すれば 32 が使われる。

## API

- `KLLSketch(capacity = 200, maximum_levels = 32, seed = 0, compare = Compare{})`（可変長版）/ `KLLSketch(seed = 0, compare = Compare{})`（固定長版）: 空で構築する。
- `add(value)`: 値を追加し、満杯になったレベルを圧縮する。
- `quantile(probability)`: 保持している値を重み付きでソートし、順位 $\lfloor p(N-1)\rfloor$（$N$ は追加総数）に当たる値を返す。
- `clear(seed = 0)`: 空に戻し、乱数生成器をシードで初期化し直す。
- `size()`: 追加した値の総数 $N$ を返す。
- `retained_items()`: 保持している値の個数 $R$ を返す。
- `capacity_per_level()`, `maximum_levels()`: $K$, $L$ を返す。
- `typical_rank_error_rate()`: $1/\sqrt{K}$ を返す。

## 時間計算量

- コンストラクタ: $O(KL)$、空間計算量 $O(KL)$
- `add`: 償却 $O(\log K)$。1 回の連鎖的な圧縮は最悪 $O(KL\log K)$、追加空間 $O(K)$
- `quantile`: $O(R\log R)$、追加空間 $O(R)$（$R\le KL$）
- `clear`, `retained_items`: $O(L)$
- その他: $O(1)$

## 注意点

- 圧縮時の偶奇選択が独立なら、順位の誤差は典型的に $O(N/\sqrt{K})$、相対誤差 $\varepsilon$ を超える確率は $e^{-\Theta(K\varepsilon^2)}$ である。
- 最上位レベルからさらに昇格が必要になると `std::overflow_error` を投げる。$N$ が 64 bit を超えた場合も同様。
- 空のスケッチへの `quantile` は `std::logic_error`、`probability` が $[0,1]$ の外なら `std::out_of_range` を投げる。
- $K$ は 2 以上の偶数、$L$ は 1 以上 63 以下でなければならない。固定長版は `static_assert`、可変長版は `std::invalid_argument` で検査する。
