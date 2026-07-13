---
title: Largest Rectangle in a Histogram (ヒストグラム最大長方形)
documentation_of: ../src/algorithm/other/largest_rectangle_histogram.hpp
---

非負の高さ列をhistogramとみなし、含まれる最大長方形を求める。

```cpp
auto result = largest_rectangle_histogram(heights);
```

`result.area` は面積、`result.left`, `result.right` は最大長方形の半開区間、
`result.height` はその高さである。空列では面積0、区間 `[0, 0)` を返す。

## 計算量

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `LargestRectangleHistogramResult` の構築・field参照 | $O(1)$ | $O(1)$ |
| `largest_rectangle_histogram(heights)` | $O(N)$ | $O(N)$ |
