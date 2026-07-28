---
title: Fully Dynamic Convex Hull (完全動的凸包)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/fully_dynamic_convex_hull.hpp
---

識別子付き点集合の上下凸包を更新のたびに維持する。全点からの遅延再構築は行わず、
x座標の乱択平衡木と、構造共有する凸包chainを用いる。同一座標へ複数の識別子を登録できる。

## API

### `bool insert(Id id, const Point& point)`

未使用の `id` と有限座標の `point` を追加する。追加した場合だけ `true` を返す。
既存 `id` の座標は変更せず `false` を返す。

### `bool erase(Id id)`

`id` の点を1個削除する。存在した場合だけ `true` を返す。同じ座標を持つ別の識別子は残る。

### `bool contains_id(Id id) const`

`id` が登録済みか返す。

### `std::size_t size() const`, `bool empty() const`

登録した識別子数と空判定を返す。座標の重複も個別に数える。

### `std::vector<Point> vertices() const`

現在の凸包頂点を、辞書順最小の頂点から反時計回りに返す。一直線上では両端だけを返す。

### `int contains(const Point& point) const`

点が凸包の外部なら0、境界なら1、内部なら2を返す。

### `std::optional<Point> support(const Point& direction) const`

`dot(point, direction)` を最大化する登録点を返す。空集合では `std::nullopt`、
零方向では辞書順最小の凸包頂点を返す。同率最大点の選択は規定しない。

## 時間計算量

登録識別子数を $N$、異なるx座標数を $U$、凸包頂点数を $H$ とする。
乱択木の期待計算量と、乱択が極端に偏った場合の最悪計算量は次の通り。

- `contains_id`, `size`, `empty`: 最悪 $O(\log N)$, $O(1)$, $O(1)$。
- 凸包の上下端を変えない同一x座標への `insert` / `erase`: 最悪 $O(\log N)$。
- 上下端を変える `insert` / `erase`: 1回あたり期待 $O(\log N+\log^4 U)$、
  期待償却も同じ。乱択木が高さ $U$ へ偏る場合の最悪は $O(\log N+U^2\log^2 U)$。
- `vertices`: 期待 $O(H+\log U)$、最悪 $O(H+U)$。
- `contains`: 期待 $O(\log U)$、最悪 $O(U)$。
- `support`: 期待 $O(\log^2 U)$、最悪 $O(U\log U)$。

格納中の空間は期待 $O(N+U\log U)$、最悪 $O(N+U^2)$。

## 注意点

同じ座標かどうか、および同じx座標かどうかの管理には `long double` の完全一致を使う。
幾何判定には共通の相対許容誤差を使う。非有限座標は `std::invalid_argument` とする。
乱択平衡木を用いるため、更新計算量の高速な境界は期待値である。
