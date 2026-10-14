---
title: Count-Min Sketch (Count-Minスケッチ)
documentation_of: ../../../src/approximate/streaming/count_min_sketch.hpp
---

## Count-Min Sketch (Count-Minスケッチ)

非負の頻度を `Width * Depth` 個の固定長 counter で近似する。

## API

- `CountMinSketch(seed)`: hash seed を指定して空で構築する。時間計算量は $O(Width\,Depth)$。 追加空間計算量は $O(1)$。
- `add(key, delta)`: 頻度を `delta` 増やす。counter は飽和加算される。時間計算量は $O(Depth)$。 追加空間計算量は $O(1)$。
- `estimate(key)`: 各 row の counter の最小値を返す。時間計算量は $O(Depth)$。 追加空間計算量は $O(1)$。
- `clear()`: 全 counter を 0 にする。時間計算量は $O(Width\,Depth)$。 追加空間計算量は $O(1)$。
- `total()`: 追加量の総和。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `width()`, `depth()`, `additive_error_rate()`, `failure_probability_bound()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

counter が飽和せず、hash が独立一様なら、真の頻度を $f_x$、総和を $N$ として
`estimate(x) >= f_x` であり、確率 $1-e^{-Depth}$ 以上で
`estimate(x) <= f_x + e N / Width`。保存に必要な空間計算量は $O(Width\,Depth)$。

## 容量を省略する可変長版

~~~cpp
CountMinSketch<> sketch(1024, 5, 123); // 幅、深さ、seed
sketch.add(42, 3);
auto frequency = sketch.estimate(42);
~~~

幅 $W$ と深さ $D$ を実行時に指定し、counter を `vector` に保存する。
構築・`clear` は $O(WD)$ 時間、追加・推定は $O(D)$ 時間。
保存領域は $O(WD)$、各操作の一時領域は $O(1)$。
容量指定の固定配列版も同じ header にある。
