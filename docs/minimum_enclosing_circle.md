---
title: Minimum Enclosing Circle (最小包含円)
documentation_of: ../src/algorithm/geometry/2d/shape/minimum_enclosing_circle.hpp
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
平行移動に安定な差分座標で「中心までの距離の2倍」と直径を比較する。3点なら差分
座標から外心を構成し、点までの距離と半径を比較する。空集合では全点が`OUTSIDE`。

## `contains` / `on_boundary`

```cpp
bool inside_or_boundary = result.contains(point);
bool boundary = result.on_boundary(point);
```

`contains`は内部または円周上、`on_boundary`は円周上の場合だけtrue。
距離比較の許容誤差は `GEOMETRY_EPS * max(点までの距離, 境界までの距離)` と
丸め誤差項からなる。半径0では中心と一致する点だけを円周上と判定する。

## `circle`

```cpp
Circle circle = result.circle();
```

supportから中心と半径を構築する。空集合では中心原点・半径0を返す。
和を先に作らず差分座標から中心を求めるため、大きな平行移動による不要な中間
overflowを避ける。中心または半径を有限な `long double` で表せない場合は
`std::overflow_error`を送出する。

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

入力座標は有限であること。円の中心・半径を有限な `long double` で表せない場合、
`minimum_enclosing_circle`, `containment`, `contains`, `on_boundary`, `circle` は
`std::overflow_error`を送出し得る。問い合わせ点までの距離だけがoverflowする場合は
円の外部として扱う。3点supportの角度が `GEOMETRY_EPS` 以下ならcollinearとして
最遠2点supportへ縮約する。

## 結果型と内部API

- `MinimumEnclosingCircleContainment`: `OUTSIDE(-1)`, `ON_BOUNDARY(0)`, `INSIDE(1)` を持つ。
- `MinimumEnclosingCircleResult::support`: 円を決める高々3点を保持する。
- `support_size`: 有効なsupport要素数で、field参照は時間計算量・空間計算量 $O(1)$。

`minimum_enclosing_circle_detail` の `next_random`、`deterministic_shuffle`、
`reduce_collinear_support` は上位関数専用で、互換性を保証しない。前者とsupport整理は
$O(1)$、長さ $N$ のshuffleは時間計算量 $O(N)$、空間計算量 $O(1)$ である。
