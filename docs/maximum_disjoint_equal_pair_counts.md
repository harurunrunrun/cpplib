---
title: Maximum Disjoint Equal Pair Counts (区間内の最大同値ペア数) [ABC242_G]
documentation_of: ../src/algorithm/other/dynamic_programming/maximum_disjoint_equal_pair_counts.hpp
---

複数の半開区間ごとに、値が等しい二要素から作れる互いに素なペアの最大個数を返す。

## maximum_disjoint_equal_pair_counts

```cpp
template<class T>
vector<int> maximum_disjoint_equal_pair_counts(
    const vector<T>& values,
    const vector<pair<int, int>>& ranges
)
```

`ranges[i] = {left, right}` に対し、
$\sum_x \lfloor \operatorname{count}(x, values[left:right])/2\rfloor$ を
`result[i]` に格納する。値は座標圧縮されるため、整数以外の比較可能な型にも使える。

## 時間計算量

$N$ を要素数、$Q$ を区間数として、
$O(N\log N + Q\log Q + (N+Q)\sqrt{\max(1,Q)})$。

## 空間計算量

$O(N+Q)$。

## 注意点

- 各区間は $0\le left\le right\le N$ の半開区間でなければならない。
- 戻り値の順序は入力した区間の順序と一致する。
- `T` はコピー、全順序比較、等値比較が可能でなければならない。
- 要素数または区間が範囲外なら `runtime_error` を送出する。
