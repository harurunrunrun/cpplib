---
title: Dynamic KD Tree (動的KD木)
documentation_of: ../../../src/structure/spatial/dynamic_kd_tree.hpp
---

部分木の再構築と遅延削除（削除フラグを立てるだけの論理削除）を用いる、次元固定の動的 KD 木である。
点の挿入・削除、直交範囲検索、ユークリッド距離の最近傍探索を扱う。

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

- `Point` は `std::array<Coordinate, DIMENSION>` である。
- `insert` は `id` が既に存在すれば何もせず `false` を、そうでなければ点を追加して `true` を返す。
- `erase` は `id` が存在すれば削除して `true` を、存在しなければ `false` を返す。
- `range_search` は各軸で `lower[axis] <= p[axis] <= upper[axis]` を満たす（閉区間の直積に含まれる）点の `id` を列挙する。結果の順序は規定しない。
- `nearest` は空集合なら `std::nullopt` を返す。`NearestResult` は `id` とユークリッド距離の二乗 `squared_distance` を持つ。距離が等しい点が複数あれば `id` が最小のものを選ぶ。

部分木の一方の子のサイズが部分木サイズの $3/4$ を超えた場合、または有効な点が半数未満になった場合に、その部分木を中央値分割で再構築する。

## 時間計算量

次元を定数とし、$N$ を点数、$K$ を列挙する点数とする。

- `empty`, `size`: $O(1)$
- `contains`: 期待 $O(1)$
- `insert`, `erase`: ならし $O(\log(N+1))$（再構築を含めたならし評価）
- `range_search`: 最悪 $O(N+K)$
- `nearest`: 最悪 $O(N)$

均衡した木と一般的な点分布では、直交範囲検索・最近傍探索の枝刈りが有効に働く。
空間計算量は、遅延削除中のノードを含めてならし $O(N)$ である。

## 注意点

- `id` は一意でなければならない（重複する `insert` は `false` を返す）。
- `lower` と `upper` がある軸で逆転していると `invalid_argument` を送出する。
- 距離は `long double` に変換して計算するため、丸め誤差を含み得る。
