---
title: Maximum Zero-Sum Subarray Lengths (区間内ゼロ和部分配列最大長) [ZQUERY]
documentation_of: ../src/algorithm/range/maximum_zero_sum_subarray_lengths.hpp
---

各問い合わせ区間に完全に含まれる、総和が $0$ の部分配列の最大長を一括計算します。

## API

### `maximum_zero_sum_subarray_lengths(values, ranges)`

```cpp
vector<int> maximum_zero_sum_subarray_lengths(
    const vector<int>& values,
    const vector<pair<int, int>>& ranges
);
```

`ranges[i]` は半開区間 `[left,right)` です。該当する非空部分配列がなければ $0$ を返します。

## 時間計算量

要素数を $N$、問い合わせ数を $Q$ として、$O((N+Q)\sqrt N+Q\log Q)$ です。

## 空間計算量

$O(N+Q)$ です。

## 注意点

- 空区間も指定でき、その答えは $0$ です。
- 区間違反では `runtime_error` を送出します。
- 接頭辞和が `long long` で表現できなければなりません。
