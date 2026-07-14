---
title: Maximum Distinct Subarray Sums (異なる値を一度だけ数える部分配列最大和) [GSS2]
documentation_of: ../src/algorithm/range/maximum_distinct_subarray_sums.hpp
---

各問い合わせ区間の部分配列について、同じ値を高々一度だけ加えた和の最大値を一括計算します。

## API

### `maximum_distinct_subarray_sums(values, ranges)`

```cpp
template<class T, int MAX_SIZE>
vector<T> maximum_distinct_subarray_sums(
    const vector<T>& values,
    const vector<pair<int, int>>& ranges
);
```

`ranges[i] = {left, right}` は半開区間 `[left, right)` を表します。返却配列の `i` 番目が対応する最大和です。

## 時間計算量

要素数を $N$、問い合わせ数を $Q$ として、期待 $O((N+Q)\log N)$ です。

## 空間計算量

$O(MAX\_SIZE+N+Q)$ です。

## 注意点

- 各問い合わせ区間は非空で、`0 <= left < right <= N` でなければなりません。
- `N <= MAX_SIZE` が必要です。
- `T` は加算、比較、ハッシュが可能でなければなりません。
- 範囲違反では `runtime_error` を送出します。
- 和が `T` で表現できる必要があります。
