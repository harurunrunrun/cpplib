---
title: Static K-th Missing Integer Set (静的k番目欠損整数集合)
documentation_of: ../../../../src/structure/ordered_set/integer/static_missing_set.hpp
---

静的な整数集合に含まれない整数を昇順に並べたときの $k$ 番目を、二分探索で求める。

## コンストラクタ

```cpp
explicit StaticMissingSet(vector<uint64_t> values, uint64_t first = 0)
```

`values` をソートして重複を除き、`first` 未満の値を捨てて保持する。

## kth_missing

```cpp
uint64_t kth_missing(uint64_t index) const
```

`first` 以上で集合に含まれない整数のうち、0-indexed で `index` 番目のものを返す。

## stored_size / first

```cpp
size_t stored_size() const
uint64_t first() const
```

保持している相異なる値の個数と、探索の開始値 `first` を返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(N\log N)$ | $O(N)$ |
| `kth_missing` | $O(\log N)$ | $O(1)$ |
| `stored_size`, `first` | $O(1)$ | $O(1)$ |

## 注意点

答えが `uint64_t` の範囲を超える場合は `std::overflow_error` を送出する。`first = 1` とすれば、正整数のうち含まれないものに関するクエリになる。
