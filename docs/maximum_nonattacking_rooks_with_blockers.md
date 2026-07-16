---
title: Maximum Nonattacking Rooks with Blockers (障害物付き非攻撃rook最大配置) [ANGELS]
documentation_of: ../src/algorithm/matching/maximum_nonattacking_rooks_with_blockers.hpp
---

障害物で視線が遮られる格子へ、互いに攻撃しないrookを置ける最大数を求めます。

## API

### `maximum_nonattacking_rooks_with_blockers(grid, placeable_cell, blocker_cell)`

```cpp
int maximum_nonattacking_rooks_with_blockers(
    const vector<string>& grid,
    char placeable_cell = '.',
    char blocker_cell = '#'
);
```

`placeable_cell` のマスだけを配置候補とし、同じ行または列で2個のrookが
`blocker_cell` を挟まず向かい合わないように選べる最大数を返します。
2種類の指定記号以外のマスは、配置できませんが視線を遮らないマスとして扱います。

## API別の時間計算量・空間計算量

格子を $R\times C$、配置候補数を $E$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_nonattacking_rooks_with_blockers` | $O(RC+E\sqrt{RC})$ | $O(RC+E)$ |

## 注意点

- `grid` は長方形でなければなりません。空の格子には `0` を返します。
- `placeable_cell` と `blocker_cell` は互いに異なる必要があります。
- 記号または形状の前提違反には `invalid_argument`、内部添字を `int` で
  表せない大きさには `length_error` を送出します。
