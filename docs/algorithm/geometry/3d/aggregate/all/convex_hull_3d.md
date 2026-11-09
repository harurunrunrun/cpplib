---
title: 3D Convex Hull (三次元凸包)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/convex_hull_3d.hpp
---

このヘッダは `convex_hull_3d_default.hpp` と `convex_hull_3d_with_seed.hpp` をまとめて include する、後方互換のための集約ヘッダである。新規コードでは必要な個別ヘッダだけを include すればよい。

## API

```cpp
ConvexPolyhedron3 convex_hull_3d(std::vector<Point3> points);
ConvexPolyhedron3 convex_hull_3d_with_seed(
    std::vector<Point3> points,
    std::uint64_t random_seed
);
```

点集合の三次元凸包を返す。座標が完全に一致する入力点は 1 つにまとめられる。

- 戻り値の `affine_dimension` は、空集合で $-1$、1 点で $0$、共線で $1$、共面で $2$、三次元の凸包で $3$ となる。
- `vertices` は凸包の極点だけを含み、面の内部の点や辺上の内部の点は含まない。
- `faces` は `vertices` の添字からなる三角形面である。共面の場合も、境界多角形を三角形分割して返す。

`convex_hull_3d` は、乱択を使わない kinetic divide-and-conquer 法を呼ぶ決定的なデフォルト API である。

`convex_hull_3d_with_seed` は、三次元入力を処理する増分順序のシードを指定する。SplitMix64 と、棄却サンプリング付きの Fisher-Yates シャッフルを使うため、同じ点集合と同じシードからは常に同じ頂点列・面列が得られる。

`convex_hull_3d_with_seed` は、各三角形面と未処理点の可視関係を双方向の conflict graph に保持する。点を追加するときは、その点と衝突する面だけを取り除き、horizon 上の各新面について、隣接していた可視面・不可視面の conflict list の和集合だけを再判定する。したがって、点を 1 つ追加するたびに生存している全面を走査することはない。

増分構築の後、共面な三角形を幾何的な面ごとにまとめ、各境界から共線な内部頂点を除いて再度三角形分割する。途中の凸包では頂点だった点が、後の挿入によって面の内部の点や辺上の内部の点になった場合も、最終出力には残らない。この正規化は面数を $H$ として時間 $O(H \log H)$、追加空間 $O(H)$ であり、下記の計算量に含まれる。

## API 別の時間計算量・空間計算量

入力点数を $N$ とする。

- `convex_hull_3d(points)`
  - すべてのアフィン次元で最悪時間 $O(N \log N)$、追加空間 $O(N)$。
  - 三次元では、左右の lower hull のイベント列を線形にマージし、2 つの向きから facet を復元する決定的な分割統治法を使う。
- `convex_hull_3d_with_seed(points, random_seed)`
  - アフィン次元が $2$ 以下: シードによらず時間 $O(N \log N)$、追加空間 $O(N)$。
  - アフィン次元が $3$: シードを入力と独立に一様ランダムに選べば、期待時間 $O(N \log N)$、期待追加空間 $O(N)$。三次元凸包の面数は $O(N)$ であり、乱択増分構築の backward analysis により、構造の変更回数は期待 $O(N)$、conflict の変更回数は期待 $O(N \log N)$ となる。順序付き連想配列による稜線の更新も期待 $O(N \log N)$ に収まる。
  - 最悪時は時間 $O(N^2 \log N)$、追加空間 $O(N^2)$。conflict の再判定回数が $32N^2$ に達した場合は走査型の増分法で最初から構築し直すため、conflict graph の履歴が悪くても三乗時間にはならない。

以上は、厳密な述語に使う多倍長整数演算のビット計算量を除いた算術演算の回数である。

## 注意点

- すべての座標は有限でなければならない。違反時は `std::invalid_argument` を送出する。
- アフィン次元と三次元の面の向きは、入力座標を正確な 2 進有理数として扱う厳密な述語で判定する。このため、巨大な平行移動量に比べて局所的な形状が非常に小さくても、`long double` に残っている差を使って判定できる。共面な凸包では、射影の前に局所的な差分を正規化する。
- 三次元の面は外向きである。同じ支持平面上の多角形面は、複数の三角形として格納される。
- 定数倍を優先して乱択増分法を使う場合に限り、入力と独立に生成したシードを `convex_hull_3d_with_seed` に渡す。デフォルト API は、敵対的な入力に対する最悪 $O(N \log N)$ の保証と再現性を優先している。
