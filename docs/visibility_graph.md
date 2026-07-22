---
title: Visibility Graph (可視グラフ)
documentation_of: ../src/algorithm/geometry/2d/shape/visibility_graph.hpp
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

入力が単純多角形であることは、Bentley–Ottmann 法で交差を高々
`N + 1` 件まで列挙して検査する。したがって検査のために全辺対を
総当たりしない。

可視辺の構築では、各始点について他の頂点を偏角順に並べる。レイと
交差する多角形辺を交点距離順の平衡二分探索木で管理する回転 sweep
により、同じ始点からの可視性を一括して判定する。始点の内角にレイが
入るかどうかも定数時間で検査する。

## 時間計算量

頂点数を $N$、返す辺数を $K$ とする。

- `visibility_graph(polygon)`: $O(N^2\log N + K)$
  - 単純多角形の入力検査は、交差数を $O(N)$ 件で打ち切る
    Bentley–Ottmann 法により $O(N\log N)$
  - 1つの始点に対する方向ソートと回転 sweep は $O(N\log N)$
  - 全始点に対する sweep は $O(N^2\log N)$
  - 返す辺の距離計算と格納は $O(K)$

$K=O(N^2)$ なので、全体の上界は $O(N^2\log N)$。

## 空間計算量

- `visibility_graph(polygon)`: $O(N^2)$
  - 返す辺は $O(K)$ 個だが、実装は最大辺数 $N(N-1)/2$ を戻り値の
    `vector` にあらかじめ `reserve` するため、確保量は $O(N^2)$
  - Bentley–Ottmann 法による入力検査と、1つの始点に対する回転
    sweep の補助領域はそれぞれ $O(N)$

## 注意点

- `polygon` は3頂点以上の有限座標からなる、面積が非零の単純多角形で
  あること。退化辺・自己交差があれば `invalid_argument`。
- 入力順は時計回り・反時計回りのどちらでもよい。
- Bentley–Ottmann 法の shear 変換を安全に行えないほど絶対値が大きい
  座標には `overflow_error`。頂点数から sweep の内部配列サイズを安全に
  計算できない場合には `length_error`。
- 第3の `polygon` 頂点を開線分上に通る頂点対は直接辺として省く。
  その頂点を経由する辺へ分割できるため、可視経路の到達可能性は
  失わない。同じレイ上に複数頂点がある場合も最も近い頂点だけを
  可視候補にする。
- 境界に沿う線分は含め、外部へ出る線分と境界を横切る線分は含めない。
- 幾何判定には `GEOMETRY_EPS` を使うため、許容誤差以下の退化配置を
  避ける。
