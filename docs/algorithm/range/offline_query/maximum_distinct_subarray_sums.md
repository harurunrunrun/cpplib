---
title: Maximum Distinct Subarray Sums (異なる値を一度だけ数える部分配列最大和) [GSS2]
documentation_of: ../../../../src/algorithm/range/offline_query/maximum_distinct_subarray_sums.hpp
---

各問い合わせ区間について、その中の部分配列のうち、同じ値は高々 1 回だけ加えるとしたときの和の最大値を一括計算する。右端の昇順に走査し、履歴最大値を持つ遅延セグメント木で求める。

## API

### `maximum_distinct_subarray_sums(values, ranges)`

```cpp
template<class T, int MAX_SIZE = -1>
vector<T> maximum_distinct_subarray_sums(
    const vector<T>& values,
    const vector<pair<int, int>>& ranges
);
```

`ranges[i] = {left, right}` は半開区間 $[\mathtt{left}, \mathtt{right})$ を表す。返り値の `i` 番目が対応する最大和である。空の部分配列（和 `T{}`）も候補に含むため、答えは `T{}` 以上になる。

`MAX_SIZE` を省略すると、内部の遅延セグメント木に実行時サイズの `std::vector` を使う可変長版になる。指定すると固定長版になる。

```cpp
auto result = maximum_distinct_subarray_sums(values, ranges);
auto fixed = maximum_distinct_subarray_sums<Value, 100000>(values, ranges);
```

## 時間計算量

要素数を $N$、問い合わせ数を $Q$ として、期待 $O(N\log N+Q\log Q+Q\log N)$（値の最終出現位置をハッシュ表で管理する）。

## 空間計算量

可変長版は $O(N+Q)$、固定長版は $O(\texttt{MAX_SIZE}+N+Q)$。

## 注意点

- 各問い合わせ区間は空でなく、`0 <= left < right <= N` でなければならない。違反時は `runtime_error` を送出する。
- 固定長版では `N <= MAX_SIZE` でなければならない。
- `T` は加算、比較、`std::hash` によるハッシュが可能でなければならない。
- 和が `T` で表現できなければならない。
