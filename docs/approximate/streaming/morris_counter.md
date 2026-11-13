---
title: Morris Counter (Morris確率カウンタ)
documentation_of: ../../../src/approximate/streaming/morris_counter.hpp
---

カウンタ値の指数だけを保持する、近似的な加算器である。

## API

- `MorrisCounter(seed = 0, base = 2.0L)`: 底 `base` で構築する。`base` は有限かつ 1 より大きくなければならず、違反すると `std::invalid_argument` を投げる。
- `increment()`: 確率 $b^{-e}$ で指数 $e$ を 1 増やす（$b$ は底）。
- `estimate()`: $(b^e-1)/(b-1)$ を返す。
- `clear(seed = 0)`: 指数を 0 に戻し、乱数生成器を初期化し直す。
- `exponent()`, `base()`: 指数 $e$（`std::uint32_t`）と底 $b$ を返す。

## 時間計算量

すべての操作が時間計算量 $O(1)$、空間計算量 $O(1)$。

## 注意点

- $n$ 回加算した後の推定値は不偏で、分散は $(b-1)n(n-1)/2$ である。複数のカウンタの平均を取れば誤差を減らせる。
- 指数が `std::uint32_t` の最大値に達すると、それ以上増えない。
