---
title: Maximum Disjoint Equal Pair Counts (区間内の最大同値ペア数) [ABC242_G]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/sequence_selection/maximum_disjoint_equal_pair_counts.hpp
---

複数の半開区間のそれぞれについて、値が等しい 2 要素から作れる、互いに要素を共有しないペアの最大個数を Mo's algorithm で求める。

## maximum_disjoint_equal_pair_counts

```cpp
template<class T>
std::vector<int> maximum_disjoint_equal_pair_counts(
    const std::vector<T>& values,
    const std::vector<std::pair<int, int>>& ranges
)
```

`ranges[i] = {left, right}` に対し、区間 $[\mathrm{left}, \mathrm{right})$ に値 $x$ が現れる回数を $c_x$ として $\sum_x \lfloor c_x/2\rfloor$ を `result[i]` に格納する。値は座標圧縮するため、整数以外の比較可能な型にも使える。

## 時間計算量

$N$ を要素数、$Q$ を区間数として、$O(N\log N + Q\log Q + N\sqrt{\max(1,Q)})$ である（Mo's algorithm のブロック幅は $N/\sqrt{Q}$）。

## 空間計算量

$O(N+Q)$。

## 注意点

- 各区間は $0\le \mathrm{left}\le \mathrm{right}\le N$ を満たす半開区間でなければならない。
- 戻り値の順序は入力した区間の順序と一致する。
- `T` はコピー、全順序比較、等値比較が可能でなければならない。
- 要素数が `int` の範囲を超える場合、または区間が範囲外の場合は `std::runtime_error` を送出する。
