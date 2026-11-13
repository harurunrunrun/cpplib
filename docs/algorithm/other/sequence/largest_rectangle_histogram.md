---
title: Largest Rectangle in a Histogram (ヒストグラム最大長方形) [DPL_3_C] [HISTOGRA]
documentation_of: ../../../../src/algorithm/other/sequence/largest_rectangle_histogram.hpp
---

非負の高さの列をヒストグラムとみなし、そこに含まれる面積最大の長方形を求める（単調スタック）。

## 使い方

```cpp
template<class T>
LargestRectangleHistogramResult<T> largest_rectangle_histogram(const vector<T>& heights);

auto result = largest_rectangle_histogram(heights);
```

`result.area` は面積、`result.left`, `result.right` は最大長方形の半開区間 $[\mathtt{left}, \mathtt{right})$、`result.height` はその高さである。面積の型 `area_type` は `std::common_type_t<T, long long>` である。面積が最大の長方形が複数あるときは、走査中に最初に見つかったものを返す。空列では面積 $0$、区間 $[0, 0)$ を返す。

## 時間計算量・空間計算量

$N$ を `heights.size()` とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `LargestRectangleHistogramResult` の構築・メンバ参照 | $O(1)$ | $O(1)$ |
| `largest_rectangle_histogram(heights)` | $O(N)$ | $O(N)$ |

## 注意点

- 負の高さが含まれると `runtime_error` を送出する。
- 面積は `area_type` で表現できなければならない。
