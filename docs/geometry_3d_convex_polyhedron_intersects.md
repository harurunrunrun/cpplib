---
title: Convex Polyhedron Intersects (凸多面体同士の交差判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/convex_polyhedron_intersects.hpp
---

## API

```cpp
bool convex_polyhedron_intersects(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
);

bool convex_polyhedron_intersects_exact(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
);
```

### `convex_polyhedron_intersects`

Minkowski差に対するGJK法で、境界接触を含め共通部分が空でないかを高速に判定する。
共通部分の頂点・面は構築せず、面列も参照しない。点・線分・平面図形を含む
空でない有限頂点集合の凸包に使用できる。

`tolerance` は入力形状の広がりに対する相対許容誤差である。距離が許容誤差以下なら
接触として扱う。反復上限到達時にも現在の最近 simplex から真偽値を返す。
収束状態も必要なら `gjk_query_3d` を使用する。

### `convex_polyhedron_intersects_exact`

完全3次元凸多面体の面法線と両多面体の辺方向の外積を分離軸として調べる。
有限な `long double` 座標を dyadic 有理数へ変換し、軸・射影・区間比較を
`ExactInteger` で厳密に行う。許容誤差や反復打切りはなく、境界接触を交差とする。
こちらも共通部分そのものは構築しない。

## API別の時間計算量・空間計算量

頂点数を $V_i$、三角形面数を $F_i$、幾何学的な辺数を $E_i$、
GJKの実行反復数を $I\leq max\_iterations$ とする。

- `convex_polyhedron_intersects`: 各 support query は全頂点を走査する。
  時間計算量は $O(F_1+F_2+I(V_1+V_2))$、空間計算量は
  $O(V_1+V_2+F_1+F_2)$ である。$F_i$ の項は内部正規化用コピーによる。
  既定の反復上限を定数とし、通常の凸多面体で $F_i=O(V_i)$ なら線形時間・線形領域である。
- `convex_polyhedron_intersects_exact`: 調べる軸数を
  $A=F_1+F_2+E_1E_2$ とすると、時間計算量は
  $O((F_1+F_2)\log(E_1+E_2)+A(V_1+V_2))$ 回の算術演算、
  空間計算量は $O(V_1+V_2+E_1+E_2)$ 個の多倍長値・添字である。
  多倍長演算のビット計算量は座標の指数差と仮数長に依存する。
  分離軸を発見した時点で終了する。

## 注意点

- 両APIとも頂点列は非空かつ有限で、各入力の凸包を表さなければならない。
- 高速版の `tolerance` は正の有限値、`max_iterations` は正でなければならず、
  違反時は `std::invalid_argument` を送出する。
- 厳密版は `affine_dimension == 3` で、閉じた凸多面体境界を三角形分割した
  正しい `faces` を必要とする。空の面列や非3次元入力は
  `std::invalid_argument`、範囲外の面頂点は `std::out_of_range`、
  有効な辺を得られない入力は `std::domain_error` となる。
- 通常は高速版を使い、許容誤差で結論が変わり得る接触判定や
  GJKの反復打切りを許容できない場合に厳密版を使う。
- 実際の共通部分が必要な場合は `convex_polyhedron_intersection` を使う。
