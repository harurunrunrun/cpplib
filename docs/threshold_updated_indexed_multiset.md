---
title: Threshold-updated Indexed Multiset (閾値更新付き添字多重集合) [TEMPLEQ]
documentation_of: ../src/structure/other/threshold_updated_indexed_multiset.hpp
---

識別番号を持つ整数列を値の昇順に管理し、1点加算、閾値以上の個数、閾値以上への一括減算を行う。

## `ThresholdUpdatedIndexedMultiset`

```cpp
template<int MAX_SIZE = 100000>
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

## Constructor

各要素に入力時の識別番号を保持したまま、値の昇順に管理できる状態を構築する。

## size

要素数を返す。

## increment

指定した識別番号の値を1増やす。

## count_at_least

値が `threshold` 以上の要素数を返す。

## decrement_at_least

値が `threshold` 以上の全要素を1減らす。

## get

指定した識別番号の現在値を返す。

## API別の時間計算量・空間計算量

要素数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `increment(index)` | $O(\log N)$ | $O(1)$ |
| `count_at_least(x)` | $O(\log N)$ | $O(1)$ |
| `decrement_at_least(x)` | $O(\log N)$ | $O(1)$ |
| `get(index)` | $O(\log N)$ | $O(1)$ |

## 注意点

- 要素数は `MAX_SIZE` 以下でなければならない。
- 識別番号は $[0,N)$ で指定する。
- 添字違反時は `runtime_error`、容量超過時は `length_error` を送出する。
- 加減算で `long long` の範囲を超える場合は `overflow_error` を送出する。
