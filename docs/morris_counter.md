---
title: Morris Counter (Morris確率カウンタ)
documentation_of: ../src/approximate/streaming/morris_counter.hpp
---

## Morris Counter (Morris確率カウンタ)

counter の指数だけを保持する近似加算器。

## API

- `MorrisCounter(seed, base)`: 有限な `base > 1` で構築する。違反は `std::invalid_argument`。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `increment()`: 確率 `base^-exponent()` で指数を増やす。期待時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `estimate()`: `(base^exponent()-1)/(base-1)`。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `clear(seed)`: 0 に戻し乱数を再初期化する。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `exponent()`, `base()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

$n$ 回の加算後の推定値は不偏で、分散は
$(base-1)n(n-1)/2$。複数 counter の平均で失敗確率を下げられる。保存に必要な空間計算量は $O(1)$。
