---
title: Maximum Nonattacking Rooks with Blockers (障害物付き非攻撃rook最大配置) [ANGELS]
documentation_of: ../../../../src/algorithm/matching/bipartite/maximum_nonattacking_rooks_with_blockers.hpp
---

障害物で利きが遮られる格子に、互いに攻撃しないルークを置ける最大数を求める。

## API

### `maximum_nonattacking_rooks_with_blockers(grid, placeable_cell, blocker_cell)`

```cpp
int maximum_nonattacking_rooks_with_blockers(
    const vector<string>& grid,
    char placeable_cell = '.',
    char blocker_cell = '#'
);
```

`placeable_cell` のマスだけを配置候補とし、同じ行または同じ列にある 2 個のルークが `blocker_cell` を挟まずに向かい合わないように選べる最大数を返す。2 種類の指定記号以外のマスは、ルークを置けないが利きを遮らないマスとして扱う。

障害物で区切られた横の区間と縦の区間を頂点とし、配置候補のマスを辺とする二部グラフの最大マッチングとして解く。

## 時間計算量・空間計算量

格子の大きさを $R \times C$、配置候補の数を $E$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_nonattacking_rooks_with_blockers` | $O(RC + E \sqrt{RC})$ | $O(RC + E)$ |

## 注意点

- `grid` は長方形でなければならない。空の格子には `0` を返す。
- `placeable_cell` と `blocker_cell` は互いに異なる必要がある。
- 記号や形状の前提違反には `invalid_argument` を、内部の添字を `int` で表せない大きさには `length_error` を送出する。
