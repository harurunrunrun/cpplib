---
title: Static Range Count and Sum at Most (静的区間上限以下個数・総和) [static_range_sum_with_upper_bound]
documentation_of: ../src/structure/wavelet_matrix/static_range_count_sum_leq.hpp
---

静的な値列について、半開区間内で指定上限以下の値の個数と総和を同時に返す。

## テンプレート引数

```cpp
StaticRangeCountSumLeq<T, W, MAX_SIZE, BIT_WIDTH>
```

`T` は整数の値型、`W` は重みと総和の型、`MAX_SIZE` は列長上限。
`BIT_WIDTH` は内部wavelet matrixが使用するbit数であり、省略時は `T` の全bit数。

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

第1形式では各値を `W` へ変換したものを重みとする。
第2形式では `values[i]` に `weights[i]` を対応させる。

## 時間計算量

$D=BIT\_WIDTH$ として $O(D\cdot MAX\_SIZE)$。

## `query`

```cpp
StaticRangeCountSumResult<W> query(int left, int right, T upper) const
```

`values[left,right)` のうち値が `upper` 以下の要素数と対応する重みの総和を返す。
`upper` が `numeric_limits<T>::max()` の場合もoverflowせず全要素を数える。

## 時間計算量

- `upper == numeric_limits<T>::max()`: $O(1)$
- それ以外: $O(D)$

## 空間計算量

queryごとの追加領域は $O(1)$。本体は $O(D\cdot MAX\_SIZE)$。

## 注意点

- `0 <= left <= right <= values.size()` が必要。空区間も許可する。
- 値列と重み列の長さは一致しなければならない。
- 無効な区間、容量超過、bit幅違反では `runtime_error`。
- `upper` が型の最大値の場合は内部で上限へ1を足さず、先に区間を検査してから長さを計算する。
- 加算overflowは `W` の演算規則に従う。
