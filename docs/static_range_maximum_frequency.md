---
title: Static Range Maximum Frequency (静的区間最大頻度) [FREQUENT]
documentation_of: ../src/structure/other/static_range_maximum_frequency.hpp
---

非減少列の任意区間で、同じ値が現れる回数の最大値を返します。

## API

### `StaticRangeMaximumFrequency(values)`

非減少な初期列から構築します。

### `size()`

要素数を返します。

### `maximum_frequency(left, right)`

非空半開区間 `[left, right)` における最大出現回数を返します。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N\log N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `maximum_frequency(l,r)` | $O(1)$ | $O(1)$ |

## 注意点

- 初期列は非減少でなければならず、違反時は `runtime_error` を送出します。
- 非空区間だけを受け付けます。
- 値の連長数が `MAX_SIZE` 以下でなければなりません。
