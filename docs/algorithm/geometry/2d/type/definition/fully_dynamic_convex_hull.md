---
title: Fully Dynamic Convex Hull (完全動的凸包)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/fully_dynamic_convex_hull.hpp
---

識別子付きの点集合に対し、挿入・削除のたびに上側・下側の凸包を維持する。全点からの再構築は行わず、
$x$ 座標をキーとする乱択平衡木（treap）の各節点に、部分木の凸包の鎖を構造共有する永続 treap で持たせる。同じ座標に複数の識別子を登録できる。

## API

### `bool insert(Id id, const Point& point)`

未使用の `id`（`Id` は `std::size_t`）と有限座標の `point` を追加する。追加した場合だけ `true` を返す。
既に使われている `id` を渡すと、その座標は変更せずに `false` を返す。

### `bool erase(Id id)`

`id` の点を 1 個削除する。存在した場合だけ `true` を返す。同じ座標をもつ別の識別子の点は残る。

### `bool contains_id(Id id) const`

`id` が登録済みかを返す。

### `std::size_t size() const`, `bool empty() const`

登録されている識別子の数と、空かどうかを返す。座標が重複する点も個別に数える。

### `std::vector<Point> vertices() const`

現在の凸包の頂点を、辞書順最小の頂点から反時計回りに返す。全点が一直線上にあるときは両端だけを返す。

### `int contains(const Point& point) const`

点が凸包の外部なら $0$、境界上なら $1$、内部なら $2$ を返す。

### `std::optional<Point> support(const Point& direction) const`

`dot(point, direction)` を最大化する登録点を返す。空集合では `std::nullopt` を返し、
零ベクトルの方向では辞書順最小の凸包頂点を返す。最大値をとる点が複数ある場合にどれを返すかは規定しない。

## 時間計算量

登録識別子数を $N$、相異なる $x$ 座標の数を $U$、凸包の頂点数を $H$ とする。
乱択木の期待計算量と、乱択が極端に偏った場合の最悪計算量は次の通り。

- `contains_id`, `size`, `empty`: それぞれ最悪 $O(\log N)$, $O(1)$, $O(1)$。
- 既存の $x$ 座標への `insert` / `erase` で、その $x$ 座標の最小・最大の $y$ 座標が変わらないもの: 最悪 $O(\log N)$。
- それ以外の `insert` / `erase`: 1 回あたり期待 $O(\log N + \log^4 U)$（期待償却でも同じ）。乱択木の高さが $U$ に偏った場合の最悪は $O(\log N + U^2 \log^2 U)$。
- `vertices`: 期待 $O(H + \log U)$、最悪 $O(H + U)$。
- `contains`: 期待 $O(\log U)$、最悪 $O(U)$。
- `support`: 期待 $O(\log^2 U)$、最悪 $O(U \log U)$。

格納に要する空間は期待 $O(N + U \log U)$、最悪 $O(N + U^2)$。

## 注意点

- 座標が同じか、$x$ 座標が同じかの判定には `long double` の完全一致を使う。幾何的な判定には共通の相対許容誤差を使う。
- 非有限な座標（`direction` を含む）には `std::invalid_argument` を送出する。
- 乱択平衡木を用いるため、更新の計算量の良い上界は期待値としてのものである。
