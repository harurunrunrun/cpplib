---
title: Minkowski Sum 3D (3次元Minkowski和)
documentation_of: ../src/algorithm/geometry/3d/minkowski_sum_3d.hpp
---

二つの凸形状のMinkowski和を `ConvexPolyhedron3` として構築する。

## API

```cpp
ConvexPolyhedron3 minkowski_sum_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
)

ConvexPolyhedron3 minkowski_sum_3d_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    uint64_t random_seed
)

ConvexPolyhedron3 minkowski_sum_3d_direct(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double general_position_margin = 1.0e-9L
)
```

- `minkowski_sum_3d` は全ての頂点対の和 $a+b$ を列挙し、
  固定seedの三次元凸包を返す。結果は実行ごとに再現可能である。
- `minkowski_sum_3d_with_seed` は同じ候補集合を使い、凸包の増分順序を
  `random_seed` で指定する。入力と独立な一様乱数seedを渡すと期待計算量を
  保証できる。

- `minkowski_sum_3d_direct` は共面三角形を一つのfacetへ統合し、
  第1項のfacetと第2項の支持頂点、第2項のfacetと第1項の支持頂点、
  および両者の幾何辺の法線錐が横断的に交差する四角形を直接つなぐ。
  全頂点対の候補点集合や、その三次元凸包は構築しない。

## API別の時間計算量・空間計算量

頂点数を $V_1,V_2$、候補数を $P=V_1V_2$、出力サイズを $K$ とする。
exact predicateの多倍長整数に対するビット計算量は除く。

直接版では、三角形面数を $T_i$、統合後のfacet数と幾何辺数を
$F_i,E_i$ とする。三角形分割内の共面对角線は $E_i$ に含めない。

- `minkowski_sum_3d`: 固定seedに対する最悪時間
  $O(P^2\log P+K)$、最悪追加領域 $O(P^2+K)$。
  既定seedに合わせた敵対的入力でなければ、通常は下記の期待計算量になる。
- `minkowski_sum_3d_with_seed`: seedを入力と独立に選んだとき期待時間
  $O(P\log P+K)$、期待追加領域 $O(P+K)$。固定seedに対する最悪上界は
  `minkowski_sum_3d` と同じ。

- `minkowski_sum_3d_direct`: 既存のfacet統合が
  $O(T_1\log T_1+T_2\log T_2)$、支持頂点探索と法線fanのoverlayが
  $O(F_1V_2+F_2V_1+E_1E_2)$、出力構築が期待 $O(K)$。
  したがって全体の期待時間は
  $O(T_1\log T_1+T_2\log T_2+F_1V_2+F_2V_1+E_1E_2+K)$、
  追加領域は $O(T_1+T_2+K)$ である。hash tableの衝突が集中した場合の
  最悪時間は線形ではない。

候補点の生成自体は時間・領域ともに $O(P)$ である。

## 注意点

- 両入力は空でない凸形状でなければならず、空入力は
  `std::invalid_argument`。
- 候補数が `vector<Point3>::max_size()` を超える場合は
  `std::length_error`。
- 非有限座標は凸包構築時に `std::invalid_argument`。
- 重複点と退化次元は凸包側で処理する。
- `minkowski_sum_3d_direct` は両入力がfull-dimensionalで、閉じた
  凸多面体境界を外向きかつ整合する三角形で表したものだけを受理する。
- 直接版では、相手の各facet法線がただ一つの頂点を支持し、異なる入力の
  全幾何辺方向についてなす角の正弦二乗が
  `general_position_margin` より大きく、交差する法線錐も境界から
  同margin以上離れている必要がある。これは高速版の
  一般位置契約であり、自己和や平行な箱などは通常この契約を満たさない。
- `general_position_margin` は有限な $(0,0.01)$ でなければならない。
  位相・向き・数値marginの検査に失敗すると `std::domain_error`、
  非有限入力や次元違いは `std::invalid_argument`、座標和のoverflowは
  `std::overflow_error`。直接版は暗黙に低速版へfallbackしない。
- 一般位置を保証できない入力では `minkowski_sum_3d` または
  `minkowski_sum_3d_with_seed` を使う。
