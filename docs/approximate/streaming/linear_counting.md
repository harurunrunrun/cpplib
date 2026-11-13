---
title: Linear Counting (線形カウンティング)
documentation_of: ../../../src/approximate/streaming/linear_counting.hpp
---

相異なるキーをハッシュでビットマップに写し、0 のままのビット数から基数（相異なるキーの個数）を推定する。

## 使い方

~~~cpp
LinearCounting<> sketch(4096, 123); // ビット数、シード
sketch.add(42);
auto cardinality = sketch.estimate();

LinearCounting<4096> fixed(123);    // ビット数をテンプレート引数で固定する版
~~~

`LinearCounting<BitCount, Key = std::uint64_t>` の `BitCount` を省略すると、ビット数 $m$ を実行時に指定して `std::vector` に $\lceil m/64\rceil$ 語を保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。

## API

- `LinearCounting(bit_count, seed = 0)`（可変長版）/ `LinearCounting(seed = 0)`（固定長版）: 空で構築する。
- `add(key)`: ハッシュ先のビットを立てる。
- `estimate()`: 0 のビット数を $V$ として $-m\log(V/m)$ を返す。0 のビットがなければ無限大を返す。
- `merge(other)`: ビットごとの OR を取る。
- `clear()`: ビットマップを消す。
- `seed()`: シードを返す。
- `occupied_bits()`, `bit_count()`: 立っているビット数、$m$ を返す。

## 時間計算量

- コンストラクタ・`merge`・`clear`: $O(\lceil m/64\rceil)$
- `add`, `estimate`, その他のアクセサ: $O(1)$
- 空間計算量: $O(\lceil m/64\rceil)$ 語

## 注意点

- シードが異なるもの同士（可変長版ではビット数が異なるもの同士も）の `merge` は `std::invalid_argument` を投げる。可変長版でビット数が 0 の場合も `std::invalid_argument`。
- 一様なハッシュの下で最尤推定量となる。負荷 $n/m$ が小さい範囲では相対誤差は $O(1/\sqrt{m})$ だが、ビットマップが飽和に近づくと分散が急増する。
