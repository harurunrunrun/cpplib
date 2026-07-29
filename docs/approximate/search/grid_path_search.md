---
title: Jump Point and Any-Angle Grid Search (ジャンプポイント・任意角グリッド探索)
documentation_of: ../../../src/approximate/search/grid_path_search.hpp
---

## 日本語

一様コストの二次元障害物グリッドに対する Jump Point Search、JPS+、Theta*、
Lazy Theta* を提供する。`GridPoint{row, column}` はセル中心、`GridMap` の
`true` セルは通行可能である。グリッド外は常に通行不能として扱う。

### 共通型

```cpp
GridMap grid(rows, columns, initially_passable);
GridMap grid(vector<string> rows, char blocked = '#');

grid.row_count();
grid.column_count();
grid.contains(point);
grid.is_passable(point);
grid.set_passable(point, passable);

enum class GridConnectivity {
    four,
    eight_corner_cut
};

struct GridPathResult {
    optional<double> cost;
    vector<GridPoint> path;
    size_t expanded;
    size_t line_of_sight_checks;
};
```

`GridMap(rows, columns, ...)` の構築時間・空間はともに \(O(HW)\)。
`contains` と `is_passable` は \(O(1)\)、`set_passable` も \(O(1)\) である。
負のサイズ、不均一な文字列長、範囲外の更新は例外となる。

`four` は上下左右のコスト 1 の移動である。`eight_corner_cut` はそれに加え、
斜めのコスト \(\sqrt 2\) の移動を許し、斜めに接する二障害物の角も横切れる。
移動モデルを暗黙にしないため、JPS の呼び出しでは列挙値を必ず指定する。

### Jump Point Search

```cpp
jump_point_search(grid, start, goal, connectivity)
```

親方向から自然近傍と強制近傍を枝刈りし、次の jump point または目標まで
一方向に走査する JPS。返す `path` は jump point だけでなく、各ジャンプ間の
全セルを含む。通行可能な始点・終点間の最短経路を返す。

展開 jump point 数を \(X\)、全ジャンプ走査で調べたセル数を \(S\) とすると、
時間計算量は \(O(S+X\log(HW))\)、追加空間は \(O(HW)\)。最悪時には同じ
ray を複数回走査するため \(S=O(HW\max(H,W))\) になり得る。

### JPS+

```cpp
JumpPointSearchPlus prepared(grid, connectivity);
prepared.grid();
prepared.connectivity();
prepared.search(start, goal);
```

各通行可能セル・各方向について、障害物までの距離と次の静的 jump event を
動的計画法で前計算する JPS+。目標固有の直進・直交曲がり・斜めから直進への
分岐は、前計算した ray 長を用いて \(O(1)\) で判定する。`search` の枝刈りと
経路復元は JPS と同じである。

構築は時間 \(O(HW)\)、空間 \(O(HW)\)。展開数 \(X\) に対する一回の探索は
時間 \(O(X\log(HW))\)、追加空間 \(O(HW)\)。構築後のグリッドは静的であり、
障害物を変更する場合は新しい `JumpPointSearchPlus` を構築する。

### Theta*

```cpp
theta_star_search(grid, start, goal)
grid_line_of_sight(grid, first, second)
```

8 近傍 A* の緩和時に、現在頂点の親から後続へ line of sight (LOS) があれば、
親から直接結ぶ。折れ点がセル中心に限定される grid path より短い任意角経路を
返せる。LOS は supercover を保守的に調べ、障害セルの角を横切らない。
`grid_line_of_sight` の時間は線分が横切るセル数に比例し、追加空間は \(O(1)\)。

生成近傍数を \(A\)、展開数を \(X\)、全 LOS が横切るセル数の合計を \(L\) と
すると、時間 \(O((A+X)\log(HW)+L)\)、空間 \(O(HW)\)。

### Lazy Theta*

```cpp
lazy_theta_star_search(grid, start, goal)
```

後続の緩和時には親からの可視性を仮定し、頂点を展開するとき初めて LOS を
検査する。仮定が誤っていれば、CLOSED の可視な隣接頂点のうち
\(g(u)+c(u,v)\) が最小のものへ親を修復する。これは Theta* の単なる別名では
なく、LOS の実行時点と探索順が異なる。

実行した LOS が横切るセル数の合計を \(L_{\rm lazy}\) とすると、
時間 \(O((A+X)\log(HW)+L_{\rm lazy})\)、空間 \(O(HW)\)。

### 注意点

- JPS/JPS+ のコストは指定した grid connectivity の厳密な最短距離。
- 4近傍では対角移動の対称性を使えないため、完全性を保つ目的で1セルずつ
  jumpするA*相当の動作に退化する。この場合の最悪時間計算量は
  \(O(HW\log(HW))\)。
- Theta* 系は任意角経路なので、8 近傍 grid path より短くなり得る。
- `cost` は `double` であり、比較には適切な許容誤差を使う。
- 始点または終点が通行不能なら、空の結果を返す。
- JPS+ は query 間で前計算を共有できるが、動的障害物には Field D* などを使う。

## English

This header provides Jump Point Search, JPS+, Theta*, and Lazy Theta* for a
uniform-cost two-dimensional obstacle grid. `GridPoint{row, column}` denotes a
cell center; an out-of-grid cell is always blocked.

`jump_point_search(grid, start, goal, connectivity)` performs directional
pruning and scans to the next forced/natural jump event. Its returned path is
expanded to every traversed grid cell. With \(X\) expanded jump points and
\(S\) scanned ray cells, it takes \(O(S+X\log(HW))\) time and \(O(HW)\) space.

`JumpPointSearchPlus(grid, connectivity)` preprocesses obstacle run lengths and
static jump events in \(O(HW)\) time and space. `search(start, goal)` then tests
every jump successor in \(O(1)\), taking \(O(X\log(HW))\) time and \(O(HW)\)
working space. The prepared map is immutable; rebuild it after an obstacle
change.

`theta_star_search` performs eager line-of-sight parent relaxation.
`lazy_theta_star_search` defers that visibility check until expansion and
repairs a false parent through a closed visible neighbor. For \(A\) generated
neighbors, \(X\) expansions, and \(L\) total raster cells examined by LOS, each
takes \(O((A+X)\log(HW)+L)\) time and \(O(HW)\) space. Lazy Theta* generally
uses a different \(L\). `grid_line_of_sight` itself takes time proportional to
the segment's supercover and \(O(1)\) space.

`GridConnectivity::four` permits only unit orthogonal moves.
In four-neighbor mode JPS/JPS+ deliberately use one-cell jumps, degenerating
to A* in \(O(HW\log(HW))\) worst-case time to preserve completeness.
`GridConnectivity::eight_corner_cut` also permits \(\sqrt2\)-cost diagonals,
including diagonals between touching blocked corners. Theta* uses conservative
no-corner-cut LOS. Blocked endpoints return no path, and floating costs should
be compared with a tolerance.
