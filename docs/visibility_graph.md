---
title: Visibility Graph (可視グラフ)
documentation_of: ../src/algorithm/geometry/2d/visibility_graph.hpp
---

単純多角形の頂点同士を、多角形の内部または境界だけを通る直接の線分で結ぶ可視グラフを構築する。

## Edge

~~~cpp
struct VisibilityGraphEdge {
    size_t first;
    size_t second;
    long double distance;
};
~~~

`first` は `second` より小さく、`distance` は2頂点間のEuclid距離。

## API

~~~cpp
vector<VisibilityGraphEdge> visibility_graph(
    const vector<Point>& polygon
)
~~~

`polygon` の全頂点を節点とする無向可視グラフの辺を返す。
多角形の隣接頂点は常に辺として含める。非隣接頂点は、その開線分が
多角形の外部を通らず、第3の多角形頂点も通らない場合に含める。
戻り値は `(first, second)` の辞書順。

各始点について、他の頂点を偏角順に並べる。レイと交差する多角形辺を
交点距離順の集合で管理する回転 sweep により、同じ始点からの可視性を
一括して判定する。始点の内角にレイが入ることも定数時間で検査する。

## 時間計算量

頂点数を $N$、返す辺数を $K$ とする。

- `visibility_graph`: $O(N^2\log N + K)$
  - 単純多角形の入力検査は $O(N^2)$
  - 各始点の方向ソートと回転 sweep は $O(N\log N)$
  - 辺の構築は $O(K)$

$K=O(N^2)$ なので、全体の上界は $O(N^2\log N)$。

## 空間計算量

- `visibility_graph`: 戻り値に $O(N^2)$、回転 sweep の作業領域に
  $O(N)$

## 注意点

- `polygon` は3頂点以上の有限座標からなる、面積が非零の単純多角形で
  あること。退化辺・自己交差があれば `invalid_argument`。
- 入力順は時計回り・反時計回りのどちらでもよい。
- 第3の `polygon` 頂点を開線分上に通る頂点対は直接辺として省く。
  その頂点を経由する辺へ分割できるため、可視経路の到達可能性は
  失わない。同じレイ上に複数頂点がある場合も最も近い頂点だけを
  可視候補にする。
- 境界に沿う線分は含め、外部へ出る線分と境界を横切る線分は含めない。
- 幾何判定には `GEOMETRY_EPS` を使うため、許容誤差以下の退化配置を
  避ける。
