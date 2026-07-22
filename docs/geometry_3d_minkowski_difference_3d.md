---
title: Minkowski Difference 3D (3次元Minkowski差)
documentation_of: ../src/algorithm/geometry/3d/minkowski_difference_3d.hpp
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
```

- `minkowski_difference_3d` は全ての頂点対の差 $a-b$ を列挙し、
  固定seedの三次元凸包を返す。原点包含は二形状の交差判定に利用できる。
- `minkowski_difference_3d_with_seed` は同じ候補集合を使い、凸包の
  増分順序を `random_seed` で指定する。入力と独立な一様乱数seedを渡すと
  期待計算量を保証できる。

## API別の時間計算量・空間計算量

頂点数を $V_1,V_2$、候補数を $P=V_1V_2$、出力サイズを $K$ とする。
exact predicateの多倍長整数に対するビット計算量は除く。

- `minkowski_difference_3d`: 固定seedに対する最悪時間
  $O(P^2\log P+K)$、最悪追加領域 $O(P^2+K)$。
  既定seedに合わせた敵対的入力でなければ、通常は下記の期待計算量になる。
- `minkowski_difference_3d_with_seed`: seedを入力と独立に選んだとき期待時間
  $O(P\log P+K)$、期待追加領域 $O(P+K)$。固定seedに対する最悪上界は
  `minkowski_difference_3d` と同じ。

候補点の生成自体は時間・領域ともに $O(P)$ である。

## 注意点

- 両入力は空でない凸形状でなければならず、空入力は
  `std::invalid_argument`。
- 候補数が `vector<Point3>::max_size()` を超える場合は
  `std::length_error`。
- 非有限座標は凸包構築時に `std::invalid_argument`。
- 重複点と退化次元は凸包側で処理する。
