---
title: Minimum Grid Character Changes (グリッド最小文字変更回数) [KATHTHI]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/specialized/minimum_grid_character_changes.hpp
---

文字のグリッド上を左上から右下まで上下左右に移動するとき、移動前後のマスの文字が異なる移動の回数の最小値を 0-1 BFS で求める。

## `minimum_grid_character_changes`

```cpp
int minimum_grid_character_changes(const std::vector<std::string>& grid)
```

## 引数

- `grid`: 各マスの文字。すべての行の長さが等しくなければならない。

## 戻り値

文字が変わる移動の回数の最小値。

## 計算量

マスの個数を $V$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_grid_character_changes` | $O(V)$ | $O(V)$ |

## 注意点

- グリッドが空（行が 0 個、または先頭行が空）の場合や、行の長さが不揃いな場合は `invalid_argument` を送出する。
- マスの個数が `int` で表現できない場合は `length_error` を送出する。
