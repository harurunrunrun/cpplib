---
title: Deterministic Divide-and-Conquer 3D Convex Hull (決定的分割統治三次元凸包)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/point_set/divide_and_conquer_convex_hull_3d.hpp
---

## 概要

有限な三次元点集合の凸包を、決定的な分割統治法で構築する。乱択増分法や、全点と全候補面の総当たりへのフォールバックは行わない。同じ点集合に対する頂点・面の順序は入力順に依存しない。

## API

```cpp
ConvexPolyhedron3 divide_and_conquer_convex_hull_3d(
    std::vector<Point3> points
)
```

重複を除いた `points` の凸包を返す。

- 空集合では `affine_dimension == -1`
- 1 点では `affine_dimension == 0`
- 共線では `affine_dimension == 1`
- 共面では `affine_dimension == 2`
- 三次元の凸包では `affine_dimension == 3`

三次元の場合、`vertices` は入力点のうち凸包の頂点だけを含む。`faces` は外向きの三角形であり、同一平面上の三角形をまとめると凸包の幾何学的な facet になる。共面の場合も境界の凸多角形だけを残し、その扇形三角形分割（fan triangulation）を `faces` に格納する。

## アルゴリズム

点を形式的に一般位置へ移したうえで、Timothy Chan の kinetic divide-and-conquer による lower hull のマージを $z$ と $-z$ に対して実行する。

各再帰でのマージは、左右の lower hull が持つイベント列を一度ずつ走査する。次の変化は、左右それぞれの次のイベントと、ブリッジの両端を 1 つ進める 4 つのイベントの合計 6 候補のいずれかである。イベントは 1 頂点の挿入または削除に対応する。子のイベントは一度だけ消費され、ブリッジの端点も hull の隣接列を単調に進むため、1 回のマージのイベント数と処理時間は部分問題の点数に対して線形である。

同一の $x$ 座標、射影上の共線、4 点以上の共面、同時イベントは、座標ごとに異なる順序付き無限小を加える Simulation of Simplicity で処理する。イベント時刻は除算せず、dyadic 整数の交差積で比較する。比較式の次数は高々 4 なので、1 つの記号的述語が持つ項数は入力点数に依存しない。通常は共通スケール上の誤差上界付き `long double` フィルタで符号を確定し、確定できない場合だけ exact 計算へ進む。

一般位置の hull を元の座標へ戻すと、共面・共線な記号的な面が生じ得る。共線でない面の支持平面を exact dyadic 係数の射影同値でまとめ、各平面に集まった点の二次元凸包を取り直す。このため、面積 0 の面の隣接関係に依存せず、各幾何学的 facet の境界を一度だけ生成する。

点数が 256 以上の場合は、固定個数（124 個）の方向それぞれの support 点から内側の凸多面体を先に作る。その全ての外向き面に対して負側にあることを `adaptive_orient3d` が厳密に証明した点だけを除去してから、分割統治を行う。内側の凸多面体は入力点の部分集合の凸包なので、元の凸包の境界点は除去されない。方向数は定数であり、この前処理は線形時間である。

## API 別の時間計算量・空間計算量

入力点数を $N$、出力の頂点・三角形面の総数を $H$ とする。三次元凸包では $H = O(N)$ である。

- `divide_and_conquer_convex_hull_3d(points)`
  - 時間: 最悪 $O(N \log N)$
  - 追加領域: $O(N)$
  - 入力の検査、重複除去、アフィン次元の判定を含む。
  - 2 つの lower hull の構築はそれぞれ $O(N \log N)$。支持平面のグループ化と facet 境界の再構築は合計 $O(H \log H)$。

exact dyadic の整数演算に必要なビット計算量は上記の RAM 計算量に含めない。1 つの述語が扱う整数・記号項の個数は点数に依存しない。

## 注意点

- 全座標は有限でなければならない。NaN または無限大を含む場合は `std::invalid_argument` を送出する。
- 内部の添字表現を安全に作れないほど点数が大きい場合は `std::length_error` を送出する。
- 座標の一致、共線、共面と面の向きは、入力された `long double` を exact dyadic 数とみなして判定する。許容誤差による点の同一視は行わない。

## 参考文献

- Timothy M. Chan,
  [A Minimalist's Implementation of the 3-d Divide-and-Conquer Convex Hull Algorithm](https://tmc.web.engr.illinois.edu/ch3d/ch3d.pdf)
