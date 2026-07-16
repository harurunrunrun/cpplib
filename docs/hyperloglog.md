---
title: HyperLogLog (基数推定)
documentation_of: ../src/approximate/streaming/hyperloglog.hpp
---

## HyperLogLog (基数推定)

stream の distinct key 数を $2^{Precision}$ 個の register で推定する。

## API

- `HyperLogLog(seed)`: hash seed を指定する。時間計算量は $O(2^{Precision})$。 追加空間計算量は $O(1)$。
- `add(key)`: key を追加する。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `estimate()`: harmonic mean と small-range correction から要素数を返す。時間計算量は $O(2^{Precision})$。 追加空間計算量は $O(1)$。
- `merge(other)`: 対応 register の最大値を取る。時間計算量は $O(2^{Precision})$。 追加空間計算量は $O(1)$。
- `clear()`: register を 0 にする。時間計算量は $O(2^{Precision})$。 追加空間計算量は $O(1)$。
- `seed()`: hash seed を返す。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `register_count()`, `relative_standard_error()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

異なる seed の merge は `std::invalid_argument`。一様な 64-bit hash を仮定する。
十分大きい cardinality で相対標準誤差は約 $1.04/\sqrt{2^{Precision}}$。
保存に必要な空間計算量は $O(2^{Precision})$ byte。
