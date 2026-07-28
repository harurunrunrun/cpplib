---
title: Dynamic KD Tree (動的KD木)
documentation_of: ../../../src/structure/spatial/dynamic_kd_tree.hpp
---

局所再構築とlazy deletionを使う固定次元dynamic KD-tree。

## API

~~~cpp
DynamicKdTree<Coordinate, DIMENSION>()
bool empty() const
size_t size() const
bool contains(int id) const
bool insert(int id, const Point& point)
bool erase(int id)
vector<int> range_search(const Point& lower, const Point& upper) const
optional<NearestResult> nearest(const Point& point) const
~~~

`NearestResult` は `id` とEuclid二乗距離 `squared_distance` を持つ。同距離は
id最小を選ぶ。range resultの順序は規定しない。

## 時間計算量

次元を固定する。

- `contains`: expected $O(1)$
- `insert`, `erase`: amortized $O(\log(N+1))$
- `range_search`: worst-case $O(N+K)$
- `nearest`: worst-case $O(N)$

均衡木と一般的な点分布ではorthogonal query/nearestの枝刈りが有効になる。
空間はlazy deletion中のnodeを含めamortized $O(N)$。

## 注意点

idは一意。逆転したrangeは `invalid_argument`。distanceは `long double`
へ変換して計算する。
