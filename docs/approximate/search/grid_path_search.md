---
title: Jump Point and Any-Angle Grid Search (ジャンプポイント・任意角グリッド探索)
documentation_of: ../../../src/approximate/search/grid_path_search.hpp
---

一様コストの二次元障害物グリッドに対する Jump Point Search、JPS+、Theta*、Lazy Theta* を提供する。`GridPoint{row, column}` はセル中心、`GridMap` の `true` のセルは通行可能である。グリッドの外は常に通行不能として扱う。以下、グリッドの行数を $H$、列数を $W$ とする。

## 共通型

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

`GridMap(rows, columns, ...)` の構築の時間計算量・空間計算量はともに $O(HW)$。`contains` と `is_passable` は $O(1)$、`set_passable` も $O(1)$ である。負のサイズ、長さが揃っていない文字列、範囲外の更新に対しては例外を送出する。

`four` は上下左右のコスト 1 の移動である。`eight_corner_cut` はそれに加え、斜めのコスト $\sqrt{2}$ の移動を許し、斜めに接する 2 つの障害物の角も横切れる。移動モデルを暗黙にしないため、JPS の呼び出しでは列挙値を必ず指定する。

## Jump Point Search

```cpp
jump_point_search(grid, start, goal, connectivity)
```

親方向から自然近傍と強制近傍を枝刈りし、次のジャンプポイントまたは目標まで一方向に走査する JPS。返す `path` はジャンプポイントだけでなく、各ジャンプ間の全セルを含む。通行可能な始点・終点間の最短経路を返す。

展開ジャンプポイント数を $X$、全ジャンプ走査で調べたセル数を $S$ とすると、時間計算量は $O(S+X\log(HW))$、追加空間計算量は $O(HW)$。最悪時には同じレイを複数回走査するため $S=O(HW\max(H,W))$ になり得る。

## JPS+

```cpp
JumpPointSearchPlus prepared(grid, connectivity);
prepared.grid();
prepared.connectivity();
prepared.search(start, goal);
```

各通行可能セル・各方向について、障害物までの距離と次の静的ジャンプイベントを動的計画法で前計算する JPS+。目標固有の直進・直交曲がり・斜めから直進への分岐は、前計算したレイ長を用いて $O(1)$ で判定する。`search` の枝刈りと経路復元は JPS と同じである。

構築は時間計算量 $O(HW)$、空間計算量 $O(HW)$。展開数 $X$ に対する 1 回の探索は時間計算量 $O(X\log(HW))$、追加空間計算量 $O(HW)$。構築後のグリッドは静的であり、障害物を変更する場合は新しい `JumpPointSearchPlus` を構築する。

## Theta*

```cpp
theta_star_search(grid, start, goal)
grid_line_of_sight(grid, first, second)
```

8 近傍 A* の緩和時に、現在頂点の親から後続へ line of sight (LOS) があれば、親から直接結ぶ。折れ点がセル中心に限定される格子経路より短い任意角経路を返せる。LOS は supercover（線分が触れる全セル）を保守的に調べ、障害セルの角を横切らない。`grid_line_of_sight` の時間計算量は線分が横切るセル数に比例し、追加空間計算量は $O(1)$。

生成近傍数を $A$、展開数を $X$、全 LOS が横切るセル数の合計を $L$ とすると、時間計算量 $O((A+X)\log(HW)+L)$、空間計算量 $O(HW)$。

## Lazy Theta*

```cpp
lazy_theta_star_search(grid, start, goal)
```

後続の緩和時には親からの可視性を仮定し、頂点を展開するとき初めて LOS を検査する。仮定が誤っていれば、CLOSED の可視な隣接頂点のうち $g(u)+c(u,v)$ が最小のものへ親を修復する。これは Theta* の単なる別名ではなく、LOS の実行時点と探索順が異なる。

実行した LOS が横切るセル数の合計を $L_{\mathrm{lazy}}$ とすると、時間計算量 $O((A+X)\log(HW)+L_{\mathrm{lazy}})$、空間計算量 $O(HW)$。

## 注意点

- JPS/JPS+ のコストは、指定した格子の連結性の厳密な最短距離である。
- 4 近傍では対角移動の対称性を使えないため、完全性を保つ目的で 1 セルずつジャンプする A* 相当の動作に退化する。この場合の最悪時間計算量は $O(HW\log(HW))$。
- Theta* 系は任意角経路なので、8 近傍の格子経路より短くなり得る。
- `cost` は `double` であり、比較には適切な許容誤差を使う。
- 始点または終点が通行不能なら、空の結果を返す。
- JPS+ は問い合わせ間で前計算を共有できるが、動的障害物には Field D* などを使う。
