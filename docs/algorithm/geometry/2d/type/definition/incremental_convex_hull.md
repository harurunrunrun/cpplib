---
title: Incremental Convex Hull (逐次追加凸包)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/incremental_convex_hull.hpp
---

整数座標の点を追加しながら、二次元の凸包を維持する。上側・下側の $x$ 単調な鎖を
それぞれ treap で管理し、新しい点から見えて不要になった頂点だけを削除する。
点を追加するたびに凸包全体を再構築することはない。

```cpp
template<std::integral Coordinate>
class IncrementalConvexHull;
```

点の型は `PointType = IntegerPoint<Coordinate>` である。面積と内積の内部計算には任意精度整数
`WideInteger = GeometryIntegerWide` を使う。

## コンストラクタ・状態

```cpp
IncrementalConvexHull()
bool empty() const
std::size_t size() const
```

空の凸包を構築する。`size` は現在の凸包の頂点数を返す。辺上にある不要な点は
頂点に含めない。

- 構築: 時間 $O(1)$、空間 $O(1)$
- `empty`: 時間 $O(1)$、追加空間 $O(1)$
- `size`: 鎖の端点を求めるため期待時間 $O(\log (H+1))$、追加空間 $O(1)$

## `insert`

```cpp
bool insert(const PointType& point)
```

`point` を追加し、凸包が変化したかを返す。同じ点の再追加や、現在の凸包の
内部・辺上にある点の追加では `false` を返す。

$k$ をこの追加で凸包から消える頂点数とする。

- 時間計算量: 期待 $O((k+1) \log (H+1))$
- 償却時間計算量: 追加列全体で 1 回あたり期待 $O(\log (N+1))$
- 追加空間計算量: 期待 $O(\log (H+1))$（再帰のスタック）

ここで $H$ は追加直前の凸包の頂点数、$N$ はそれまでに追加した点の数である。各点は各鎖
から高々 1 回しか削除されない。

## 内外判定

```cpp
int contains(const PointType& point) const
```

点が凸包の外部なら `0`、境界上なら `1`、内部なら `2` を返す。空集合や、1 点・
線分に退化した凸包にも対応する。

- 時間計算量: 期待 $O(\log (H+1))$
- 追加空間計算量: $O(1)$

## 面積

```cpp
WideInteger doubled_area() const
```

凸包の面積の 2 倍を、非負の任意精度整数で返す。

- 時間計算量: 期待 $O(\log (H+1))$（各部分木に保持した辺の外積の和を使う）
- 追加空間計算量: 戻り値を除き $O(1)$

## 支持点

```cpp
std::optional<PointType> support(const PointType& direction) const
```

`point.x * direction.x + point.y * direction.y` を最大化する凸包頂点を返す。
複数ある場合は辞書順最小の頂点を返す。空なら `std::nullopt` を返す。

- 時間計算量: 期待 $O(\log (H+1))$
- 追加空間計算量: $O(1)$

## 頂点列

```cpp
std::vector<PointType> vertices() const
```

凸包の頂点を、辞書順最小の頂点から反時計回りに返す。辺上の中間点と、末尾に重ねた始点は
含めない。線分に退化した場合は 2 端点を辞書順に返す。

- 時間計算量: $O(H)$
- 追加空間計算量: 戻り値を含め $O(H)$

## 注意点

- 追加だけを扱い、削除は扱わない。`Coordinate` は整数型でなければならない。
- treap の高さに関する計算量は、内部の擬似乱数（鎖ごとに固定シードの splitmix64）で決まる優先度に対する期待値である。
- 保存領域は $O(H)$。座標差・外積・面積は任意精度で計算するため、符号付き整数の
  オーバーフローを起こさない。
