---
title: Deterministic Divide-and-Conquer 3D Convex Hull (決定的分割統治三次元凸包)
documentation_of: ../src/algorithm/geometry/3d/shape/divide_and_conquer_convex_hull_3d.hpp
---

## 概要

有限な三次元点集合の凸包を、決定的な分割統治法で構築する。
乱択 incremental construction や全点と全候補面の総当たりには
fallback しない。同じ点集合に対する頂点・面の順序は入力順に依存しない。

## API

```cpp
ConvexPolyhedron3 divide_and_conquer_convex_hull_3d(
    std::vector<Point3> points
)
```

重複を除いた `points` の凸包を返す。

- 空集合では `affine_dimension == -1`
- 一点では `affine_dimension == 0`
- 共線では `affine_dimension == 1`
- 共面では `affine_dimension == 2`
- 三次元の凸包では `affine_dimension == 3`

三次元の場合、`vertices` は入力点のうち凸包の頂点だけを含む。
`faces` は外向きの三角形であり、同一平面上の三角形をまとめると
凸包の幾何学的な facet になる。共面の場合も境界凸多角形だけを残し、
fan triangulation を `faces` に格納する。

## アルゴリズム

点を形式的に一般位置へ移した上で、Timothy Chan の kinetic
divide-and-conquer lower-hull merge を $z$ と $-z$ に対して実行する。

各再帰 merge は、左右の lower hull が持つ event 列を一度ずつ走査する。
次の変化は、左右それぞれの次 event と、bridge の両端を一つ進める
四つの event の合計六候補のいずれかである。event は一頂点の
挿入または削除に対応する。子の event は一度だけ消費され、bridge の端点も
hull の隣接列を単調に進むため、一回の merge の event 数と処理時間は
部分問題の点数に対して線形である。

同一 $x$、投影上の共線、四点以上の共面、同時 event は、座標ごとに異なる
順序付き無限小を加える Simulation of Simplicity で処理する。
event 時刻は除算せず、dyadic 整数の交差積で比較する。
比較式の次数は高々四なので、一つの symbolic predicate が持つ項数は
入力点数に依存しない。通常は共通スケール上の誤差上界付き
`long double` filter で符号を確定し、確定できない場合だけ exact 計算へ進む。

一般位置の hull を元座標へ戻すと、共面・共線な symbolic face が生じ得る。
非共線 face の支持平面を exact dyadic 係数の射影同値でまとめ、
各平面に集まった点の二次元凸包を取り直す。このため、零面積 face の隣接関係に
依存せず、各幾何 facet の境界を一度だけ生成する。

## API別の時間計算量・空間計算量

入力点数を $N$、出力頂点・三角形面の総数を $H$ とする。
三次元凸包では $H=O(N)$ である。

- `divide_and_conquer_convex_hull_3d(points)`
  - 時間: 最悪 $O(N\log N)$
  - 追加領域: $O(N)$
  - 入力の検査、重複除去、アフィン次元判定を含む。
  - 二つの lower hull は各 $O(N\log N)$。支持平面の grouping と
    facet 境界の再構築は合計 $O(H\log H)$。

exact dyadic の整数演算に必要なビット計算量は上記の RAM 計算量に含めない。
一つの predicate が扱う整数・symbolic 項の個数は点数に依存しない。

## 注意点

全座標は有限でなければならない。NaN または無限大を含む場合は
`std::invalid_argument` を送出する。内部の添字表現を安全に作れないほど
点数が大きい場合は `std::length_error` を送出する。

座標の一致、共線、共面と面の向きは、入力された `long double` を
exact dyadic 数とみなして判定する。許容誤差による点の同一視は行わない。

## 参考文献

- Timothy M. Chan,
  [A Minimalist's Implementation of the 3-d Divide-and-Conquer Convex Hull Algorithm](https://tmc.web.engr.illinois.edu/ch3d/ch3d.pdf)
