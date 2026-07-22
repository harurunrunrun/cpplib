---
title: Minimum Grid Character Changes (グリッド最小文字変更回数) [KATHTHI]
documentation_of: ../src/algorithm/graph/shortest_path/minimum_grid_character_changes.hpp
---

## minimum_grid_character_changes

```cpp
int minimum_grid_character_changes(const vector<string>& grid)
```

左上から右下まで上下左右に移動するとき、移動前後の文字が異なる回数の最小値を返す。

## 引数

- `grid`: 各マスの文字。行長はすべて等しいものとする。

## 戻り値

文字が変わる移動回数の最小値。

## API別の時間計算量・空間計算量

マス数を $V$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_grid_character_changes` | $O(V)$ | $O(V)$ |

## 注意点

- 空または長さが不揃いなグリッドでは `invalid_argument` を送出する。
- マス数が `int` で表現できない場合は `length_error` を送出する。
