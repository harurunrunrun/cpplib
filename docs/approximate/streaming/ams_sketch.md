---
title: AMS Sketch (AMSスケッチ)
documentation_of: ../../../src/approximate/streaming/ams_sketch.hpp
---

turnstile ストリーム（正負の更新を許すストリーム）の第 2 周波数モーメント $F_2=\sum_x f_x^2$ を推定する。各標本は 4-wise independent な符号ハッシュによる符号付き和の二乗で、グループ内では平均、グループ間では中央値を取る。

## 使い方

~~~cpp
AMSSketch<> sketch(9, 32, 123); // グループ数、グループごとの標本数、シード
sketch.add(42, 3);
auto second_moment = sketch.estimate_second_moment();

AMSSketch<9, 32> fixed(123);    // 容量をテンプレート引数で固定する版
~~~

テンプレート引数 `AMSSketch<Groups, SamplesPerGroup, Key = std::uint64_t>` を省略した `AMSSketch<>` は、グループ数 $G$ と標本数 $S$ を実行時に指定して `std::vector` に保存する可変長版である。`Groups`, `SamplesPerGroup` を指定すると `std::array` に保存する固定長版になる。キーは `std::hash<Key>` で 64 bit 整数に変換する。

## API

- `AMSSketch(groups, samples_per_group, seed = 0)`（可変長版）/ `AMSSketch(seed = 0)`（固定長版）: シードから符号ハッシュの係数を生成する。
- `add(key, delta = 1.0L)`: `key` の頻度を `delta`（`long double`）だけ変化させ、全標本に反映する。
- `estimate_second_moment()`: グループ平均の中央値を返す。グループ数が偶数のときは中央の 2 値の平均。
- `clear()`: 符号付き和をすべて 0 に戻す（係数は保つ）。
- `groups()`, `samples_per_group()`: グループ数・標本数を返す。

## 時間計算量

グループ数を $G$、グループごとの標本数を $S$ とする。

- コンストラクタ、`add`、`clear`: $O(GS)$
- `estimate_second_moment`: 平均 $O(GS)$（中央値は `std::nth_element`）、追加空間 $O(G)$
- `groups`, `samples_per_group`: $O(1)$
- 空間計算量: $O(GS)$

## 注意点

- 4-wise independent な符号の理論では、単一標本は $F_2$ の不偏推定量で分散は高々 $2F_2^2$ である。グループ平均に Chebyshev の不等式、グループ間の中央値に Chernoff bound を適用でき、$S=O(1/\varepsilon^2)$, $G=O(\log(1/\delta))$ で相対誤差 $\varepsilon$、失敗確率 $\delta$ となる。
- 可変長版で `groups` または `samples_per_group` が 0 なら `std::invalid_argument`、$GS$ が `std::size_t` に収まらなければ `std::length_error` を投げる。固定長版では 0 は `static_assert` で弾かれる。
