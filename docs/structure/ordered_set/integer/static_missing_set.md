---
title: Static K-th Missing Integer Set (静的k番目欠損整数集合)
documentation_of: ../../../../src/structure/ordered_set/integer/static_missing_set.hpp
---

静的集合に含まれない整数を昇順に並べたときの $k$ 番目を二分探索する。

## constructor

```cpp
explicit StaticMissingSet(vector<uint64_t> values, uint64_t first = 0)
```

重複を除去し、`first` 未満を捨てる。

## kth_missing

```cpp
uint64_t kth_missing(uint64_t index) const
```

`first` 以上の欠損整数のうち 0 始まり `index` 番目を返す。

## stored_size / first

```cpp
size_t stored_size() const
uint64_t first() const
```

保持した異なる値数と探索開始値を返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(N\log N)$ | $O(N)$ |
| `kth_missing` | $O(\log N)$ | $O(1)$ |
| `stored_size`, `first` | $O(1)$ | $O(1)$ |

## 注意点

答えが `uint64_t` を超える場合は `overflow_error` を送出する。`first=1` とすれば正整数の欠損クエリになる。
