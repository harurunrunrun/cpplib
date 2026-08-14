---
title: Sliding Window MEX (固定長窓MEX)
documentation_of: ../../../../src/algorithm/range/sliding_window/window_mex.hpp
---

固定長の全窓の MEX、またはその最小値を求める。

## sliding_window_mexes

```cpp
vector<size_t> sliding_window_mexes(const vector<int64_t>& values, size_t window)
```

各固定長窓の MEX を左端順に返す。

## minimum_fixed_window_mex

```cpp
size_t minimum_fixed_window_mex(const vector<int64_t>& values, size_t window)
```

出現位置間の gap を使い、全窓の MEX の最小値だけを線形時間で返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sliding_window_mexes` | $O(N\log K)$ | $O(K)$（戻り値を除く） |
| `minimum_fixed_window_mex` | $O(N)$ | $O(N)$ |

## 注意点

`1 <= window <= values.size()` が必要で、違反時は `out_of_range` を送出する。
