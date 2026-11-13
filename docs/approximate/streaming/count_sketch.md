---
title: Count Sketch (Countスケッチ)
documentation_of: ../../../src/approximate/streaming/count_sketch.hpp
---

正負の更新を許す turnstile ストリームの各キーの頻度を、符号付きハッシュと中央値で近似する。

## 使い方

~~~cpp
CountSketch<> sketch(1024, 5, 123); // 幅、深さ、シード
sketch.add(42, -3);
auto frequency = sketch.estimate(42);

CountSketch<1024, 5> fixed(123);    // 容量をテンプレート引数で固定する版
~~~

`CountSketch<Width, Depth, Counter = std::int64_t, Key = std::uint64_t>` の `Width`, `Depth` を省略すると、幅 $W$ と深さ $D$ を実行時に指定して `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。`Counter` は符号付き整数型。

## API

- `CountSketch(width, depth, seed = 0)`（可変長版）/ `CountSketch(seed = 0)`（固定長版）: 空で構築する。
- `add(key, delta = 1)`: `key` の頻度に `delta` を加える。
- `estimate(key)`: 行ごとに符号を補正したカウンタ値の中央値を返す。深さが偶数のときは中央の 2 値の平均（各値を 2 で割ってから足す）。
- `clear()`: 全カウンタを 0 にする。
- `width()`, `depth()`: 幅・深さを返す。

## 時間計算量

- コンストラクタ・`clear`: $O(WD)$
- `add`: $O(D)$、追加空間 $O(1)$
- `estimate`: 平均 $O(D)$（中央値は `std::nth_element`）、追加空間 $O(D)$
- 空間計算量: $O(WD)$

## 注意点

- 列ハッシュと符号ハッシュが独立という理論モデルでは、1 行の推定の標準偏差は $O(\lVert f_{\ne x}\rVert_2/\sqrt{W})$ である。中央値を取ることで、定数倍の誤差を超える確率は $e^{-\Theta(D)}$ まで下がる。
- カウンタは表現範囲で飽和する。理論保証は飽和が起きない更新列に対するものである。
- 可変長版で幅または深さが 0 なら `std::invalid_argument`、$WD$ が `std::size_t` に収まらなければ `std::length_error` を投げる。
