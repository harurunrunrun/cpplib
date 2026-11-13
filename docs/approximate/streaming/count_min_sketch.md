---
title: Count-Min Sketch (Count-Minスケッチ)
documentation_of: ../../../src/approximate/streaming/count_min_sketch.hpp
---

非負の頻度を、幅 $W$・深さ $D$ の $WD$ 個のカウンタで近似する。

## 使い方

~~~cpp
CountMinSketch<> sketch(1024, 5, 123); // 幅、深さ、シード
sketch.add(42, 3);
auto frequency = sketch.estimate(42);

CountMinSketch<1024, 5> fixed(123);    // 容量をテンプレート引数で固定する版
~~~

`CountMinSketch<Width, Depth, Counter = std::uint64_t, Key = std::uint64_t>` の `Width`, `Depth` を省略すると、幅と深さを実行時に指定して `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。`Counter` は符号なし整数型。

## API

- `CountMinSketch(width, depth, seed = 0)`（可変長版）/ `CountMinSketch(seed = 0)`（固定長版）: ハッシュのシードを指定して空で構築する。
- `add(key, delta = 1)`: `key` の頻度を `delta` 増やす。カウンタは飽和加算される。
- `estimate(key)`: 各行の対応するカウンタの最小値を返す。
- `clear()`: 全カウンタと総和を 0 にする。
- `total()`: 追加量の総和（飽和加算）を返す。
- `width()`, `depth()`: 幅・深さを返す。
- `additive_error_rate()`: $e/W$ を返す。
- `failure_probability_bound()`: $e^{-D}$ を返す。

## 時間計算量

- コンストラクタ・`clear`: $O(WD)$
- `add`, `estimate`: $O(D)$、追加空間 $O(1)$
- その他: $O(1)$
- 空間計算量: $O(WD)$

## 注意点

- カウンタが飽和せずハッシュが独立一様なら、真の頻度を $f_x$、総和を $N$ として $\mathrm{estimate}(x)\ge f_x$ であり、確率 $1-e^{-D}$ 以上で $\mathrm{estimate}(x)\le f_x+eN/W$ が成り立つ。
- 可変長版で幅または深さが 0 なら `std::invalid_argument`、$WD$ が `std::size_t` に収まらなければ `std::length_error` を投げる。
