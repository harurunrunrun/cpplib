---
title: Threshold-updated Indexed Multiset (閾値更新付き添字多重集合) [TEMPLEQ]
documentation_of: ../../../../src/structure/ordered_set/transform_multiset/threshold_updated_indexed_multiset.hpp
---

識別番号を持つ整数列を値の昇順に管理し、1 点への 1 加算、閾値以上の要素数の取得、閾値以上の全要素への一括 1 減算を行う。内部では値の昇順に並べた列を遅延セグメント木（区間加算・区間最大値）で管理する。

## `ThresholdUpdatedIndexedMultiset`

```cpp
template<int MAX_SIZE = -1>
class ThresholdUpdatedIndexedMultiset {
public:
    explicit ThresholdUpdatedIndexedMultiset(const vector<long long>& values);
    int size() const;
    void increment(int original_index);
    int count_at_least(long long threshold) const;
    void decrement_at_least(long long threshold);
    long long get(int original_index) const;
};
```

`MAX_SIZE` は要素数の上限である。省略時（`-1`）は上限を設けず、実行時の要素数に合わせて領域を確保する。

## コンストラクタ

`values[i]` に識別番号 `i` を付け、値の昇順に管理できる状態を構築する。

## size

要素数を返す。

## increment

識別番号 `original_index` の要素の値を 1 増やす。

## count_at_least

値が `threshold` 以上の要素数を返す。

## decrement_at_least

値が `threshold` 以上の全要素を 1 減らす。

## get

識別番号 `original_index` の要素の現在値を返す。

## API 別の時間計算量・空間計算量

要素数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `increment(index)` | $O(\log N)$ | $O(1)$ |
| `count_at_least(x)` | $O(\log N)$ | $O(1)$ |
| `decrement_at_least(x)` | $O(\log N)$ | $O(1)$ |
| `get(index)` | $O(\log N)$ | $O(1)$ |

保存領域は $O(N)$ である。

## 注意点

- `MAX_SIZE` を指定した場合、要素数は `MAX_SIZE` 以下でなければならない。超えた場合（または `INT_MAX` を超えた場合）はコンストラクタが `std::length_error` を送出する。
- 識別番号は $[0, N)$ で指定する。範囲外の場合は `std::runtime_error` を送出する。
- 加減算で `long long` の範囲を超える場合は `std::overflow_error` を送出する。
