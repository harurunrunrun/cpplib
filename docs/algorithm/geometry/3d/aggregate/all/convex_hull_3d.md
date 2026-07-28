---
title: 3D Convex Hull (三次元凸包)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/convex_hull_3d.hpp
---

このheaderは `convex_hull_3d_default.hpp` と `convex_hull_3d_with_seed.hpp`
をまとめてincludeする後方互換aggregatorである。新規コードでは必要なleafだけをincludeできる。

## API

```cpp
ConvexPolyhedron3 convex_hull_3d(vector<Point3> points)
ConvexPolyhedron3 convex_hull_3d_with_seed(
    vector<Point3> points,
    uint64_t random_seed
)
```

点集合の三次元凸包を返す。座標が完全一致する入力点は一つにまとめられる。
返り値の `affine_dimension` は、空集合で $-1$、一点で $0$、共線で $1$、
共面で $2$、三次元の凸包で $3$ となる。`vertices` は凸包の極点だけを含み、
面内部点と辺上内部点は含まない。`faces` は `vertices` の添字からなる
三角形面である。共面の場合も境界多角形を三角形分割して返す。

`convex_hull_3d_with_seed` は、三次元入力を処理する増分順序のseedを指定する。
SplitMix64とrejection sampling付きFisher-Yates shuffleを使うため、同じ点集合と
同じseedからは常に同じ頂点列・面列が得られる。`convex_hull_3d` は
乱択を使わないkinetic divide-and-conquer法を呼ぶ決定的な既定APIである。

`convex_hull_3d_with_seed` は、各三角形面と未処理点の可視関係を
双方向のconflict graphに保持する。
点を追加するときは、その点と衝突する面だけを除き、horizon上の各新面について、
隣接していた可視面・不可視面のconflict listの和だけを再判定する。したがって、
各追加点から全生存面を走査しない。

増分構築後は共面三角形を幾何面ごとにまとめ、各境界から共線な内部頂点を除いて
再度三角形分割する。途中の凸包で頂点だった点が、後の挿入によって面内部点や
辺上内部点になった場合も最終出力には残らない。この正規化は面数を $H$ として
時間 $O(H\log H)$、追加領域 $O(H)$ であり、下記の計算量に含まれる。

## API別の時間計算量・空間計算量

入力点数を $N$ とする。

- `convex_hull_3d(points)`
  - 全アフィン次元: 最悪時間 $O(N\log N)$、追加領域 $O(N)$。
  - 三次元では左右lower hullのevent列を線形mergeし、二つの向きから
    facetを復元する決定的分割統治法を使う。
- `convex_hull_3d_with_seed(points, random_seed)`
  - アフィン次元が $2$ 以下: seedによらず時間 $O(N\log N)$、追加領域 $O(N)$。
  - アフィン次元が $3$: seedを入力と独立に一様ランダムに選ぶと期待時間
    $O(N\log N)$、期待追加領域 $O(N)$。三次元凸包の面数が $O(N)$ であり、
    randomized incremental constructionのbackward analysisにより構造変更数が
    期待 $O(N)$、conflict変更数が期待 $O(N\log N)$ となる。ordered mapによる
    稜線更新も期待 $O(N\log N)$ に収まる。最悪時には時間 $O(N^2\log N)$、追加領域
    $O(N^2)$。conflict再判定が $32N^2$ 回へ達した場合は走査型増分法で最初から
    再構築するため、conflict graphの悪い履歴によって三乗時間にはならない。

上記はexact predicateに使う多倍長整数演算のビット計算量を除いた算術演算回数である。

## 注意点

全座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
アフィン次元と三次元面の向きは入力座標の exact dyadic predicate で判定するため、
巨大な平行移動量に比べて局所形状が非常に小さくても、`long double` に残っている差を
使って判定する。共面凸包の射影前には局所差分を正規化する。三次元の面は外向きであり、
同じ支持平面上の多角形面は複数の三角形として格納される。

定数倍を優先して乱択増分法を使う場合だけ、入力から独立に生成したseedを
`convex_hull_3d_with_seed` へ渡す。既定APIは敵対的入力に対する
最悪 $O(N\log N)$ 保証と再現性を優先する。
