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

bool convex_polyhedron_intersects_exact_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
);

bool convex_polyhedron_intersects_exact_sat(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
);
```

### `convex_polyhedron_intersects`

Minkowski差に対するGJK法で、境界接触を含め共通部分が空でないかを
許容誤差付きで判定する。共通部分の頂点・面は構築せず、面列も参照しない。
点・線分・平面図形を含む空でない有限頂点集合の凸包に使用できる。

反復上限へ達した場合も現在の最近simplexから真偽値を返すため、このAPIは
有限時間で終了する実用的な近似判定であり、厳密判定ではない。収束状態も
必要なら `gjk_query_3d` を使用する。

### `convex_polyhedron_intersects_exact`

両多面体の三角形面を厳密な半空間へ変換し、その共通部分の実行可能性を
固定次元のランダム化増分法で判定する。入力の最初の多面体のAABBを表す
6制約を固定prefixとして先に処理し、残りの面制約だけを入力と独立に
shuffleする。各再帰段では最小Euclidノルム解を新しい境界平面、次に
二平面の交線へ制限する。交線上の実行可能区間は一回の走査で求める。

半空間係数、射影点、交線パラメータは共通分母を持つ
`ExactInteger` のhomogeneous座標で保持する。浮動小数点除算、
許容誤差、反復打切りを使わず、面・辺・頂点だけの接触も交差とする。
乱数は実行時間だけに影響し、返り値には影響しない。

shuffle用engineは `std::random_device` から生成する。

### `convex_polyhedron_intersects_exact_with_seed`

厳密増分法の制約順を `random_seed` で指定する再現可能版である。
seedを入力と独立な一様乱数として選べば期待線形時間になる。固定seedに
合わせて構成された入力に対して線形時間は保証しない。

### `convex_polyhedron_intersects_exact_sat`

面法線と両多面体の幾何辺方向の外積を全て分離軸として調べる決定的な
厳密版である。有限な `long double` 座標をdyadic有理数へ変換し、
軸・射影・区間比較を `ExactInteger` で行う。ランダム性を使わないため、
小さい入力のoracleや増分版との比較検証に利用できる。

## API別の時間計算量・空間計算量

頂点数を $V_i$、三角形面数を $F_i$、幾何辺数を $E_i$、
$H=F_1+F_2$、GJKの実行反復数を
$I\leq\mathtt{max\_iterations}$ とする。以下では
`ExactInteger` 一回の演算を $O(1)$ と数える。

- `convex_polyhedron_intersects`: 時間
  $O(F_1+F_2+I(V_1+V_2))$、追加領域
  $O(V_1+V_2+F_1+F_2)$。反復上限を定数にすれば実行時間は線形だが、
  そのことはGJKの収束または返り値の厳密性を保証しない。
- `convex_polyhedron_intersects_exact`: 入力独立なランダム順に対して
  期待時間 $O(V_1+V_2+H)$、追加領域 $O(V_1+V_2+H)$。
  増分順が最悪の場合の時間上界は $O(V_1+V_2+H^3)$。
- `convex_polyhedron_intersects_exact_with_seed`: seedが入力と独立なら
  期待時間・領域は上と同じ。固定seedに対する最悪時間は
  $O(V_1+V_2+H^3)$。
- `convex_polyhedron_intersects_exact_sat`: 軸数を
  $A=F_1+F_2+E_1E_2$ とすると、時間
  $O((F_1+F_2)\log(E_1+E_2)+A(V_1+V_2))$ 回の算術演算、
  追加領域 $O(V_1+V_2+E_1+E_2)$。

多倍長演算のビット計算量は、座標の指数差と生成される整数のlimb数に依存する。

## 注意点

- `convex_polyhedron_intersects` の両頂点列は空でなく有限で、各入力の
  凸包を表す必要がある。`tolerance` は正の有限値、
  `max_iterations` は正でなければならず、違反時は
  `std::invalid_argument`。
- 三つの厳密APIは `affine_dimension == 3` の有限頂点と、
  完全3次元凸多面体の閉じた境界を表す非退化三角形面を必要とする。
  面の向きと列挙順は問わず、各面を全頂点の厳密な重心側が内側になるよう
  正規化する。
- 空の頂点・面列または非3次元入力は `std::invalid_argument`、
  範囲外の面頂点は `std::out_of_range`、退化面または空間的な
  多面体を囲まない面は増分版で `std::domain_error` となる。
- SAT版は有効な幾何辺を得られない入力、非多様体辺を持つmeshに
  `std::domain_error` を送出する。重複した同一半空間を許す増分版とは
  mesh検証の契約が異なる。
- `convex_polyhedron_intersects_exact` では
  `std::random_device` の生成失敗がそのまま送出され得る。再現性または
  外部乱数源を避けたい場合はseed付き版かSAT版を使う。
- 実際の共通部分が必要な場合は
  `convex_polyhedron_intersection` を使う。

## 参考文献

- Raimund Seidel, [Small-Dimensional Linear Programming and Convex Hulls Made Easy](https://doi.org/10.1007/BF02574699), 1991.
- Pankaj K. Agarwal, Sariel Har-Peled, Nabil H. Mustafa, Yusu Wang,
  [Optimal Detection of Intersections between Convex Polyhedra](https://arxiv.org/abs/1312.1001), 2015.
