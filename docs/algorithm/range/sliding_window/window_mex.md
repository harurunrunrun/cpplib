---
title: Sliding Window MEX (固定長窓MEX)
documentation_of: ../../../../src/algorithm/range/sliding_window/window_mex.hpp
---

長さ `window` の全窓の MEX、またはその最小値を求める。
MEX は窓に含まれない最小の非負整数である。負の値や `window` を超える値は MEX に影響しないため無視される。

## sliding_window_mexes

```cpp
vector<size_t> sliding_window_mexes(const vector<int64_t>& values, size_t window)
```

各窓 `[i, i + window)` の MEX を左端 `i` の昇順に返す。
内部では値域 $\lbrace 0,\dots,K\rbrace$ の `MexMultiset` を用いる。

## minimum_fixed_window_mex

```cpp
size_t minimum_fixed_window_mex(const vector<int64_t>& values, size_t window)
```

各値の出現位置の間隔を調べ、全窓の MEX の最小値だけを線形時間で返す。

## 時間計算量

$N$ を `values.size()`、$K$ を `window` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `sliding_window_mexes` | $O(N\log K)$ | $O(K)$（戻り値を除く） |
| `minimum_fixed_window_mex` | $O(N)$ | $O(N)$ |

## 注意点

`1 <= window <= values.size()` が必要で、違反時は `std::out_of_range` を送出する。
