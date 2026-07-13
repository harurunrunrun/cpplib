---
title: Z Algorithm (Zアルゴリズム)
documentation_of: ../src/algorithm/string/z_algorithm.hpp
---

各位置からの接頭辞との最長共通接頭辞長を求める。

## 関数

- `z_algorithm(s)`
    - `z[0] = s.size()`。

## 計算量

- $O(n)$

## API別計算量

| API | 時間計算量 | 出力領域 |
| --- | --- | --- |
| `z_algorithm(s)` | $O(N)$ | $O(N)$ |

長さ $N$ のZ-arrayを列挙して返す。
