---
title: Maximum Zero-Sum Subarray Lengths (区間内ゼロ和部分配列最大長) [ZQUERY]
documentation_of: ../../../../src/algorithm/range/offline_query/maximum_zero_sum_subarray_lengths.hpp
---

各問い合わせ区間に完全に含まれる、総和が $0$ の部分配列の最大長を一括計算する。接頭辞和を座標圧縮し、Mo 法と平方分割で同じ接頭辞和の最遠ペアを管理する。

## API

### `maximum_zero_sum_subarray_lengths(values, ranges)`

```cpp
vector<int> maximum_zero_sum_subarray_lengths(
    const vector<int>& values,
    const vector<pair<int, int>>& ranges
);
```

`ranges[i] = {left, right}` は半開区間 $[\mathtt{left},\mathtt{right})$ を表す。該当する空でない部分配列がなければ $0$ を返す。

## 時間計算量

要素数を $N$、問い合わせ数を $Q$ として、$O((N+Q)\sqrt N+Q\log Q)$。

## 空間計算量

$O(N+Q)$。

## 注意点

- 空区間も指定でき、その答えは $0$ である。
- `0 <= left <= right <= N` を満たさない区間では `runtime_error` を送出する。
- 接頭辞和が `long long` で表現できなければならない。
