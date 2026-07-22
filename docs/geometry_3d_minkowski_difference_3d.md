---
title: Minkowski Difference 3D (3次元Minkowski差)
documentation_of: ../src/algorithm/geometry/3d/shape/minkowski_difference_3d.hpp
---

二つの凸形状について $\{a-b\mid a\in A,b\in B\}$ の凸包を構築する。

## API

```cpp
ConvexPolyhedron3 minkowski_difference_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
)

ConvexPolyhedron3 minkowski_difference_3d_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    uint64_t random_seed
)

ConvexPolyhedron3 minkowski_difference_3d_direct(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double general_position_margin = 1.0e-9L
)
```

- `minkowski_difference_3d` は全ての頂点対の差 $a-b$ を列挙し、
  固定seedの三次元凸包を返す。原点包含は二形状の交差判定に利用できる。
- `minkowski_difference_3d_with_seed` は同じ候補集合を使い、凸包の
  増分順序を `random_seed` で指定する。入力と独立な一様乱数seedを渡すと
  期待計算量を保証できる。

- `minkowski_difference_3d_direct` は第2項の頂点を反転し、面向きを
  反転してから、一般位置用の直接Minkowski和構築を再利用する。
  共面三角形分割の対角線は幾何辺から除外され、全頂点対の候補点集合や
  その三次元凸包は構築しない。

## API別の時間計算量・空間計算量

頂点数を $V_1,V_2$、候補数を $P=V_1V_2$、出力サイズを $K$ とする。
exact predicateの多倍長整数に対するビット計算量は除く。

直接版では、三角形面数を $T_i$、統合後のfacet数と幾何辺数を
$F_i,E_i$ とする。三角形分割内の共面对角線は $E_i$ に含めない。

- `minkowski_difference_3d`: 固定seedに対する最悪時間
  $O(P^2\log P+K)$、最悪追加領域 $O(P^2+K)$。
  既定seedに合わせた敵対的入力でなければ、通常は下記の期待計算量になる。
- `minkowski_difference_3d_with_seed`: seedを入力と独立に選んだとき期待時間
  $O(P\log P+K)$、期待追加領域 $O(P+K)$。固定seedに対する最悪上界は
  `minkowski_difference_3d` と同じ。

- `minkowski_difference_3d_direct`: 第2項の反転が $O(V_2+T_2)$、
  facet統合が $O(V_1+V_2+T_1+T_2)$、支持頂点探索と
  法線fanのoverlayが $O(F_1V_2+F_2V_1+E_1E_2)$、
  出力構築が期待 $O(K)$。入力サイズを
  $N_i=V_i+T_i+E_i+F_i$ とすると全体の期待時間は
  $O(T_1+T_2+F_1V_2+F_2V_1+E_1E_2+K)=O(N_1N_2+K)$、
  追加領域は $O(V_1+V_2+T_1+T_2+K)$ である。出力構築用hash tableの
  衝突が集中した場合の最悪時間は線形ではない。

候補点の生成自体は時間・領域ともに $O(P)$ である。

## 注意点

- 両入力は空でない凸形状でなければならず、空入力は
  `std::invalid_argument`。
- 候補数が `vector<Point3>::max_size()` を超える場合は
  `std::length_error`。
- 非有限座標は凸包構築時に `std::invalid_argument`。
- 重複点と退化次元は凸包側で処理する。
- `minkowski_difference_3d_direct` は両入力がfull-dimensionalで、
  閉じた凸多面体境界を外向きかつ整合する三角形で表したものだけを受理する。
- 直接版では、反転後の相手の各facet法線がただ一つの頂点を支持し、
  異なる入力の全幾何辺方向についてなす角の正弦二乗が
  `general_position_margin` より大きく、交差する法線錐も境界から
  同margin以上離れている必要がある。
- `general_position_margin` は有限な $(0,0.01)$ でなければならない。
  位相・向き・数値marginの検査に失敗すると `std::domain_error`、
  非有限入力や次元違いは `std::invalid_argument`、座標差のoverflowは
  `std::overflow_error`。直接版は暗黙に低速版へfallbackしない。
- 一般位置を保証できない入力では `minkowski_difference_3d` または
  `minkowski_difference_3d_with_seed` を使う。
