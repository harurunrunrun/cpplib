---
title: 3D Convex Hull, Legacy Divide-and-Conquer API (三次元凸包・旧分割統治API)
documentation_of: ../src/algorithm/geometry/3d/shape/divide_and_conquer_convex_hull_3d.hpp
---

## API

```cpp
ConvexPolyhedron3 divide_and_conquer_convex_hull_3d(
    vector<Point3> points
)
```

点集合の三次元凸包を `ConvexPolyhedron3` として返す。
`affine_dimension` は空集合、一点、共線、共面、三次元に対してそれぞれ
$-1,0,1,2,3$ となる。三次元の `faces` は外向きの三角形面であり、
共面の場合は境界凸多角形を三角形分割する。

この名前は旧APIとのソース互換性のために残している。旧実装は再帰mergeごとに
全ての辺・頂点の組を支持面候補として全点検査していたため、入力が増えると
三乗時間になった。現在は `convex_hull_3d` と同じ、面と未処理点の可視関係を
conflict graphで管理するrandomized incremental constructionを使う。
既定seedが固定されているため、同じ入力からは常に同じ頂点列・面列を返す。

構築後は共面三角形を幾何面ごとにまとめ、その境界を二次元凸包として取り直す。
したがって、構築途中で一時的に頂点となった面内部点や辺上内部点は
`vertices` に残らない。

## API別の時間計算量・空間計算量

入力点数を $N$ とする。

- アフィン次元が $2$ 以下: 時間 $O(N\log N)$、追加領域 $O(N)$。
- アフィン次元が $3$: 入力順と独立なランダムseedに対する期待時間
  $O(N\log N)$、期待追加領域 $O(N)$。この互換APIは再現性のため固定seedを使うので、
  保証される最悪時間は $O(N^2\log N)$、最悪追加領域は $O(N^2)$ である。
  conflict再判定回数に上限を設けており、悪い履歴でも三乗時間へは戻らない。

exact predicateの多倍長整数演算に要するビット計算量は上記に含めない。

## 注意点

座標が完全一致する点は一つにまとめる。全座標は有限でなければならず、
違反時は `std::invalid_argument` を送出する。次元判定と三次元面の向きには
入力された `long double` のexact dyadic predicateを使う。API名とは異なり、
現在の内部アルゴリズムは分割統治ではない。
