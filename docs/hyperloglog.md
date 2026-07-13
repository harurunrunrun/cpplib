---
title: HyperLogLog (基数推定)
documentation_of: ../src/approximate/streaming/hyperloglog.hpp
---

stream の distinct key 数を $2^{Precision}$ 個の register で推定する。

- `HyperLogLog(seed)`: hash seed を指定する。$O(2^{Precision})$。
- `add(key)`: key を追加する。$O(1)$。
- `estimate()`: harmonic mean と small-range correction から要素数を返す。$O(2^{Precision})$。
- `merge(other)`: 対応 register の最大値を取る。$O(2^{Precision})$。
- `clear()`: register を 0 にする。$O(2^{Precision})$。
- `seed()`: hash seed を返す。$O(1)$。
- `register_count()`, `relative_standard_error()`: $O(1)$。

異なる seed の merge は `std::invalid_argument`。一様な 64-bit hash を仮定する。
十分大きい cardinality で相対標準誤差は約 $1.04/\sqrt{2^{Precision}}$。
保存領域は $O(2^{Precision})$ byte。
