---
title: Z Algorithm (Zアルゴリズム) [zalgorithm]
documentation_of: ../src/algorithm/string/z_algorithm.hpp
---

各位置からの接頭辞との最長共通接頭辞長を求める。

## 関数

- `z_algorithm(s)`
    - `z[0] = s.size()`。

## 計算量の概要

- $O(n)$

## API別の時間計算量・空間計算量

| API | 時間計算量 | 出力領域 |
| --- | --- | --- |
| `z_algorithm(s)` | $O(N)$ | $O(N)$ |

長さ $N$ のZ-arrayを列挙して返す。

## 注意点

文字範囲、alphabet、固定容量、構築後の更新可否は各APIの説明に従う。返すindexと区間は0-indexedである。
