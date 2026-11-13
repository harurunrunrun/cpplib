---
title: MinHash (最小ハッシュ)
documentation_of: ../../../src/approximate/streaming/minhash.hpp
---

集合の Jaccard 類似度を、長さ $S$ の固定長シグネチャで推定する。

## 使い方

~~~cpp
MinHash<> first(128, 123), second(128, 123); // シグネチャ長、シード
first.add(42);
second.add(42);
auto similarity = first.similarity(second);

MinHash<128> fixed(123);                      // シグネチャ長をテンプレート引数で固定する版
~~~

`MinHash<SignatureSize, Key = std::uint64_t>` の `SignatureSize` を省略すると、シグネチャ長 $S$ を実行時に指定して `std::vector` に保存する可変長版になる。指定すると `std::array` に保存する固定長版になる。

## API

- `MinHash(signature_size, seed = 0)`（可変長版）/ `MinHash(seed = 0)`（固定長版）: 空集合として構築する。
- `add(key)`: 各ハッシュ成分の最小値を更新する。
- `similarity(other)`: 一致する成分の割合を返す。両方空なら 1、片方だけ空なら 0。
- `merge(other)`: 成分ごとの最小値を取り、和集合を表す状態にする。
- `clear()`: 空集合に戻す。
- `signature()`: シグネチャへの const 参照を返す（コピーしない）。
- `seed()`: シードを返す。
- `empty()`, `signature_size()`: 空かどうか、$S$ を返す。

## 時間計算量

- コンストラクタ・`add`・`similarity`・`merge`・`clear`: $O(S)$
- `signature`, `seed`, `empty`, `signature_size`: $O(1)$
- 空間計算量: $O(S)$

## 注意点

- シードが異なるもの同士（可変長版ではシグネチャ長が異なるもの同士も）の比較・`merge` は `std::invalid_argument` を投げる。可変長版で $S=0$ の場合も `std::invalid_argument`。
- 独立な置換という理論モデルでは、推定値の期待値は Jaccard 係数 $J$、標準偏差は $\sqrt{J(1-J)/S}$ である。Hoeffding の不等式より $\Pr[\lvert\hat J-J\rvert\ge\varepsilon]\le2e^{-2S\varepsilon^2}$。
