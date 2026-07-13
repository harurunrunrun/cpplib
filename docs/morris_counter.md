---
title: Morris Counter
documentation_of: ../src/approximate/streaming/morris_counter.hpp
---

counter の指数だけを保持する近似加算器。

- `MorrisCounter(seed, base)`: 有限な `base > 1` で構築する。違反は `std::invalid_argument`。$O(1)$。
- `increment()`: 確率 `base^-exponent()` で指数を増やす。期待 $O(1)$。
- `estimate()`: `(base^exponent()-1)/(base-1)`。$O(1)$。
- `clear(seed)`: 0 に戻し乱数を再初期化する。$O(1)$。
- `exponent()`, `base()`: $O(1)$。

$n$ 回の加算後の推定値は不偏で、分散は
$(base-1)n(n-1)/2$。複数 counter の平均で失敗確率を下げられる。保存領域は $O(1)$。
