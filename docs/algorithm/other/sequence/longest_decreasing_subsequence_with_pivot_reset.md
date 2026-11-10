---
title: Longest Decreasing Subsequence with Pivot Reset (基準位置で条件を一度解除できる最長減少部分列) [BAT3]
documentation_of: ../../../../src/algorithm/other/sequence/longest_decreasing_subsequence_with_pivot_reset.hpp
---

列の狭義減少部分列について、指定位置の要素を選んだ場合に限り、その次に選ぶ 1 要素だけ大小条件を解除できるときの最大長を返す。指定位置を使わない通常の減少部分列も候補に含む。

## longest_decreasing_subsequence_with_pivot_reset

```cpp
int longest_decreasing_subsequence_with_pivot_reset(
    const vector<int>& values,
    int pivot_index
);
```

`pivot_index` の要素を選んだ場合、その直後に選ぶ要素へは値によらず遷移できる。それ以外の連続する選択要素は、添字が増加し、値が真に減少しなければならない。

## 時間計算量・空間計算量

$N$ を `values.size()` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `longest_decreasing_subsequence_with_pivot_reset` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `values` は空であってはならない。
- `pivot_index` は 0-indexed で $[0, N)$ に含まれなければならない。
- 長さを `int` で表せない入力には `length_error` を、範囲外の `pivot_index` には `runtime_error` を送出する。
