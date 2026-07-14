---
title: Minimum Enclosing Circle (最小包含円)
documentation_of: ../src/algorithm/geometry/2d/minimum_enclosing_circle.hpp
---

点集合をすべて含む最小半径の円を、固定seedのrandomized incremental法で求める。
結果は円を決定する高々3点のsupportとして保持する。

## `minimum_enclosing_circle`

```cpp
MinimumEnclosingCircleResult result = minimum_enclosing_circle(points);
MinimumEnclosingCircleResult result = minimum_enclosing_circle(points, seed);
```

点列を値渡しで受け取り、shuffle後に最小包含円を求める。同じ入力とseedには
同じsupportを返す。空集合では`support_size == 0`、1点では1、直径で決まる円では
2、非collinearな3点の外接円で決まる場合は3となる。重複点を含んでもよい。

## `MinimumEnclosingCircleResult::containment`

```cpp
auto relation = result.containment(point);
```

`OUTSIDE`, `ON_BOUNDARY`, `INSIDE`のいずれかを返す。supportが2点なら直径円の
内積判定、3点ならincircle determinantを使い、中心や半径を構築せず判定する。
空集合の結果では全点が`OUTSIDE`。

## `contains` / `on_boundary`

```cpp
bool inside_or_boundary = result.contains(point);
bool boundary = result.on_boundary(point);
```

`contains`は内部または円周上、`on_boundary`は円周上の場合だけtrue。
比較には`geometry_sign`と`GEOMETRY_EPS`を使う。

## `circle`

```cpp
Circle circle = result.circle();
```

supportから中心と半径を構築する。空集合では中心原点・半径0を返す。
判定だけが必要なら、丸めを一段増やさない`containment`を使う。

## API別の時間計算量・空間計算量

`N = points.size()`とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `minimum_enclosing_circle` | 期待 $O(N)$、最悪 $O(N^3)$ | $O(N)$ |
| `containment` | $O(1)$ | $O(1)$ |
| `contains` / `on_boundary` | $O(1)$ | $O(1)$ |
| `circle` | $O(1)$ | $O(1)$ |

期待計算量は入力順をshuffleするrandomized incremental法に対するもの。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。

## 結果型と内部API

- `MinimumEnclosingCircleContainment`: `OUTSIDE(-1)`, `ON_BOUNDARY(0)`, `INSIDE(1)` を持つ。
- `MinimumEnclosingCircleResult::support`: 円を決める高々3点を保持する。
- `support_size`: 有効なsupport要素数で、field参照は時間計算量・空間計算量 $O(1)$。

`minimum_enclosing_circle_detail` の `next_random`、`deterministic_shuffle`、
`reduce_collinear_support` は上位関数専用で、互換性を保証しない。前者とsupport整理は
$O(1)$、長さ $N$ のshuffleは時間計算量 $O(N)$、空間計算量 $O(1)$ である。
