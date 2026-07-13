---
title: Count-Min Sketch
documentation_of: ../src/approximate/streaming/count_min_sketch.hpp
---

非負の頻度を `Width * Depth` 個の固定長 counter で近似する。

- `CountMinSketch(seed)`: hash seed を指定して空で構築する。$O(Width\,Depth)$。
- `add(key, delta)`: 頻度を `delta` 増やす。counter は飽和加算される。$O(Depth)$。
- `estimate(key)`: 各 row の counter の最小値を返す。$O(Depth)$。
- `clear()`: 全 counter を 0 にする。$O(Width\,Depth)$。
- `total()`: 追加量の総和。$O(1)$。
- `width()`, `depth()`, `additive_error_rate()`, `failure_probability_bound()`: $O(1)$。

counter が飽和せず、hash が独立一様なら、真の頻度を $f_x$、総和を $N$ として
`estimate(x) >= f_x` であり、確率 $1-e^{-Depth}$ 以上で
`estimate(x) <= f_x + e N / Width`。保存領域は $O(Width\,Depth)$。
