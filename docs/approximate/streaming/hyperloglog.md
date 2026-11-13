---
title: HyperLogLog (基数推定)
documentation_of: ../../../src/approximate/streaming/hyperloglog.hpp
---

ストリーム中の相異なるキーの個数を、$M=2^P$ 個のレジスタで推定する（$P$ は precision）。

## 使い方

~~~cpp
HyperLogLog<> sketch(12, 123); // precision、シード
sketch.add(42);
auto cardinality = sketch.estimate();

HyperLogLog<12> fixed(123);    // precision をテンプレート引数で固定する版
~~~

`HyperLogLog<Precision, Key = std::uint64_t>` の `Precision` を省略すると、$P$ を実行時に指定してレジスタを `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。

## API

- `HyperLogLog(precision, seed = 0)`（可変長版）/ `HyperLogLog(seed = 0)`（固定長版）: ハッシュのシードを指定して空で構築する。
- `add(key)`: キーを追加する。
- `estimate()`: 調和平均による推定値に、小さい範囲の補正（linear counting）と 64 bit ハッシュ空間に対する大きい範囲の補正をかけて返す。
- `merge(other)`: 対応するレジスタの最大値を取り、2 つのストリームの和集合を表す状態にする。
- `clear()`: レジスタを 0 にする。
- `seed()`: シードを返す。
- `register_count()`: $M$ を返す。
- `relative_standard_error()`: $1.04/\sqrt{M}$ を返す。

## 時間計算量

- コンストラクタ・`estimate`・`merge`・`clear`: $O(M)$
- `add`, `seed`, `register_count`, `relative_standard_error`: $O(1)$
- 空間計算量: $O(M)$ バイト

## 注意点

- $P$ は 4 以上 18 以下でなければならない。固定長版は `static_assert`、可変長版は `std::invalid_argument` で検査する。
- シードが異なるもの同士（可変長版では precision が異なるもの同士も）の `merge` は `std::invalid_argument` を投げる。
- 一様な 64 bit ハッシュを仮定する。十分大きい基数で相対標準誤差は約 $1.04/\sqrt{M}$ である。
