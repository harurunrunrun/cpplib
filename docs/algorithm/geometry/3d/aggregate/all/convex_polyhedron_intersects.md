---
title: Convex Polyhedron Intersects (凸多面体同士の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/convex_polyhedron_intersects.hpp
---

このヘッダは GJK 版・乱択厳密版・シード指定厳密版・厳密 SAT 版の個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。新規コードでは必要な判定法の個別ヘッダだけを include すればよい。

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

Minkowski 差に対する GJK 法で、境界での接触を含めて共通部分が空でないかを、許容誤差付きで判定する。共通部分の頂点・面は構築せず、面の列も参照しない。点・線分・平面図形を含め、空でない有限な頂点集合の凸包に対して使える。

反復回数の上限に達した場合も、その時点の最近 simplex から真偽値を返す。したがって、この API は有限時間で終了する実用的な近似判定であり、厳密な判定ではない。収束状態も必要なら `gjk_query_3d` を使う。

### `convex_polyhedron_intersects_exact`

両多面体の三角形面を厳密な半空間に変換し、その共通部分が空でないか（実行可能性）を、固定次元のランダム化増分法で判定する。最初の多面体の AABB を表す 6 個の制約を固定の接頭辞として先に処理し、残りの面の制約だけを入力と独立にシャッフルする。各再帰段では、最小ユークリッドノルム解を新しい境界平面上、次に 2 平面の交線上に制限して求める。交線上の実行可能区間は 1 回の走査で求める。

半空間の係数、射影点、交線のパラメータは、共通の分母を持つ `ExactInteger` の同次座標で保持する。浮動小数点の除算、許容誤差、反復の打ち切りを使わず、面・辺・頂点だけで接する場合も交差とみなす。乱数は実行時間にだけ影響し、戻り値には影響しない。

シャッフル用の乱数エンジンは `std::random_device` から生成する。

### `convex_polyhedron_intersects_exact_with_seed`

厳密増分法の制約の順序を `random_seed` で指定する、再現可能な版である。シードを入力と独立な一様乱数として選べば期待線形時間になる。固定シードに合わせて構成された入力に対しては、線形時間を保証しない。

### `convex_polyhedron_intersects_exact_sat`

面の法線と、両多面体の幾何的な辺の方向同士の外積をすべて分離軸として調べる、決定的な厳密版である。有限な `long double` 座標を 2 進有理数に変換し、軸・射影・区間の比較を `ExactInteger` で行う。乱択を使わないため、小さい入力に対する正解判定や、増分版との比較検証に利用できる。

## API 別の時間計算量・空間計算量

頂点数を $V_i$、三角形面数を $F_i$、幾何的な辺の数を $E_i$、$H = F_1 + F_2$、GJK の実際の反復回数を $I$（$I \le \texttt{max_iterations}$）とする。以下では `ExactInteger` の 1 回の演算を $O(1)$ と数える。

- `convex_polyhedron_intersects`: 時間 $O(F_1 + F_2 + I(V_1 + V_2))$、追加空間 $O(V_1 + V_2 + F_1 + F_2)$。反復回数の上限を定数にすれば実行時間は線形だが、GJK の収束や戻り値の厳密性は保証されない。
- `convex_polyhedron_intersects_exact`: 入力と独立なランダム順序に対して期待時間 $O(V_1 + V_2 + H)$、追加空間 $O(V_1 + V_2 + H)$。増分順序が最悪の場合の時間の上界は $O(V_1 + V_2 + H^3)$。
- `convex_polyhedron_intersects_exact_with_seed`: シードが入力と独立なら、期待時間・空間は上と同じ。固定シードに対する最悪時間は $O(V_1 + V_2 + H^3)$。
- `convex_polyhedron_intersects_exact_sat`: 軸の数を $A = F_1 + F_2 + E_1 E_2$ とすると、時間 $O((F_1 + F_2) \log (E_1 + E_2) + A(V_1 + V_2))$ 回の算術演算、追加空間 $O(V_1 + V_2 + E_1 + E_2)$。

多倍長演算のビット計算量は、座標の指数の差と、生成される整数の limb 数に依存する。

## 注意点

- `convex_polyhedron_intersects` の両頂点列は空でなく有限で、各入力の凸包を表している必要がある。`tolerance` は正の有限値、`max_iterations` は正でなければならない。違反時は `std::invalid_argument` を送出する。
- 3 つの厳密 API は、`affine_dimension == 3` で有限な頂点と、完全に三次元の凸多面体の閉じた境界を表す非退化な三角形面を必要とする。面の向きと列挙順は問わず、各面を全頂点の厳密な重心の側が内側になるよう正規化する。
- 頂点列・面列が空、または三次元でない入力には `std::invalid_argument`、面の頂点添字が範囲外なら `std::out_of_range` を送出する。退化した面や、空間的に多面体を囲まない面の集合に対しては、増分版が `std::domain_error` を送出する。
- SAT 版は、有効な幾何的な辺を得られない入力や、非多様体の辺を持つメッシュに対して `std::domain_error` を送出する。同一の半空間の重複を許す増分版とは、メッシュ検証の契約が異なる。
- `convex_polyhedron_intersects_exact` では、`std::random_device` の生成失敗による例外がそのまま送出され得る。再現性が必要な場合や外部の乱数源を避けたい場合は、シード付き版か SAT 版を使う。
- 実際の共通部分が必要な場合は `convex_polyhedron_intersection` を使う。

## 参考文献

- Raimund Seidel, [Small-Dimensional Linear Programming and Convex Hulls Made Easy](https://doi.org/10.1007/BF02574699), 1991.
- Pankaj K. Agarwal, Sariel Har-Peled, Nabil H. Mustafa, Yusu Wang, [Optimal Detection of Intersections between Convex Polyhedra](https://arxiv.org/abs/1312.1001), 2015.
