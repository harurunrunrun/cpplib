---
title: Static Range Maximum Frequency (静的区間最大頻度) [FREQUENT]
documentation_of: ../../../../src/structure/range_query/value_query/static_range_maximum_frequency.hpp
---

非減少な列の任意の区間で、同じ値が現れる回数の最大値を返す。

## API

```cpp
template<class T, int MAX_SIZE = -1>
class StaticRangeMaximumFrequency;
```

`MAX_SIZE` は値の連（同じ値が連続する部分）の個数の上限であり、省略時（`-1`）は上限を設けない。

### `StaticRangeMaximumFrequency(values)`

非減少な初期列から構築する。

### `size()`

要素数を返す。

### `maximum_frequency(left, right)`

空でない半開区間 `[left, right)` における、値の出現回数の最大値を返す。

## API 別の時間計算量・空間計算量

要素数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `maximum_frequency(l, r)` | $O(1)$ | $O(1)$ |

## 注意点

- 初期列は非減少でなければならず、違反時は `std::runtime_error` を送出する。
- 空でない区間 `0 <= left < right <= size()` だけを受け付け、違反時は `std::runtime_error` を送出する。
- 値の連の個数が `MAX_SIZE` 以下でなければならない。
