---
title: Visible Lattice Points (可視格子点数) [VLATTICE]
documentation_of: ../src/algorithm/math/visible_lattice_points.hpp
---

立方格子の原点から、途中に別の格子点を挟まず見える点を数える前計算表。

# `VisibleLatticePointsTable`

```cpp
explicit VisibleLatticePointsTable(int limit);
```

`1..limit` の Möbius 関数累積和を構築する。

# `limit`

```cpp
int limit() const;
```

表の上限を返す。

# `value`

```cpp
uint64_t value(int n) const;
```

`0..n` の三次元格子で、原点以外から原点が見える点の個数を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(limit)$ | $O(limit)$ |
| `limit` | $O(1)$ | $O(1)$ |
| `value` | $O(\sqrt n)$ | $O(1)$ |

## 注意点

- `1 <= n <= limit` が必要である。
