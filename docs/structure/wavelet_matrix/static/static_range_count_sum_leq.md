---
title: Static Range Count and Sum at Most (静的区間上限以下個数・総和) [static_range_sum_with_upper_bound]
documentation_of: ../../../../src/structure/wavelet_matrix/static/static_range_count_sum_leq.hpp
---

静的な整数列について、半開区間内で指定した上限以下の値の個数と、それらの重みの総和を同時に返す。内部で `WeightedWaveletMatrix` を使う。

## テンプレート引数

```cpp
StaticRangeCountSumLeq<T, W, MAX_SIZE = -1, BIT_WIDTH = (T の bit 数)>
```

- `T`: 値の整数型
- `W`: 重みと総和の型
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `BIT_WIDTH`: 内部のウェーブレット行列が使う bit 数。省略時は `T` の bit 数。

```cpp
using Variable = StaticRangeCountSumLeq<T, W>;
using Fixed = StaticRangeCountSumLeq<T, W, N>;
```

## 結果型

```cpp
template<class W>
struct StaticRangeCountSumResult {
    int count;
    W sum;
};
```

## コンストラクタ

```cpp
explicit StaticRangeCountSumLeq(const vector<T>& values)
StaticRangeCountSumLeq(const vector<T>& values, const vector<W>& weights)
```

1 つ目の形式では、各値を `W` へ変換したものを重みとする。2 つ目の形式では、`values[i]` に重み `weights[i]` を対応させる。

## query

```cpp
StaticRangeCountSumResult<W> query(int left, int right, T upper) const
```

`values[left, right)` のうち値が `upper` 以下の要素の個数と、それらの重みの総和を返す。`upper` が `numeric_limits<T>::max()` の場合もオーバーフローせず全要素を数える。

## 時間計算量

$N$ を列長、$D = \texttt{BIT_WIDTH}$ とする。

- コンストラクタ: 可変長版は $O(D N)$、固定長版は $O(D \cdot \texttt{MAX_SIZE})$
- `query`: `upper == numeric_limits<T>::max()` なら $O(1)$、それ以外は $O(D)$

## 空間計算量

本体は $O(D N)$（固定長版では $O(D \cdot \texttt{MAX_SIZE})$）。クエリごとの追加領域は $O(1)$。

## 注意点

- $0 \le \mathit{left} \le \mathit{right} \le N$ が必要である。空区間も許す。
- 値列と重み列の長さは一致しなければならない。
- 無効な区間、容量超過、値が `BIT_WIDTH` bit に収まらない場合は `std::runtime_error` を送出する。
- `upper` が型の最大値の場合は内部で上限に 1 を足さず、先に区間を検査してから長さを計算する。
- `BIT_WIDTH` を `T` の bit 数より小さくした場合も、`upper` が $2^{\texttt{BIT_WIDTH}} - 1$ 以上なら区間内のすべての要素を数える。
- 加算のオーバーフローは `W` の演算規則に従う。
