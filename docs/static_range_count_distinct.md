---
title: Static Range Count Distinct
documentation_of: ../src/algorithm/range/static_range_count_distinct.hpp
---

静的配列の半開区間に含まれる相異なる値の個数を求める。各位置について同じ値の直前の出現位置を作り、それをWavelet Matrixで数える。

# テンプレート引数・構築

```cpp
StaticRangeCountDistinct<T, MAX_SIZE, Hash, KeyEqual> structure(values)
```

- `T`: 配列要素型
- `MAX_SIZE`: 配列長上限
- `Hash`, `KeyEqual`: 直前出現位置の構築に使う

配列長が`MAX_SIZE`を超える場合は例外を送出する。

## 時間計算量

- expected $O(N)$

hash tableのworst caseでは$O(N^2)$。Wavelet Matrixのbit幅は`int`のbit数で固定される。

# 区間query

```cpp
int count(int l, int r) const
int count_distinct(int l, int r) const
```

`values[l:r]`に現れる相異なる値の個数を返す。`count_distinct`は`count`の別名。空区間では0を返す。範囲外は例外。

## 時間計算量

- $O(W)$

$W$は`int`のbit数で、通常32。

# 要素数

```cpp
int size() const
```

## 時間計算量

- $O(1)$

# 空間計算量

- $O(NW)$ bit
