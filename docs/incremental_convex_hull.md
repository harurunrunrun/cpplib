---
title: Incremental Convex Hull (逐次追加凸包)
documentation_of: ../src/algorithm/geometry/2d/incremental_convex_hull.hpp
---

整数座標の点を追加しながら、2次元凸包を保つ。上側・下側の $x$ 単調chainを
randomized treapで管理し、新しい点から見えて不要になった頂点だけを削除する。
点を追加するたびに凸包全体を再構築しない。

```cpp
template<std::integral Coordinate>
class IncrementalConvexHull;
```

点の型は `IntegerPoint<Coordinate>`、面積と内積の内部計算には任意精度整数
`GeometryIntegerWide` を使う。

## 構築・状態

```cpp
IncrementalConvexHull()
bool empty() const
std::size_t size() const
```

空の凸包を構築する。`size` は現在の凸包の頂点数を返す。辺上の不要な点は
頂点に含めない。

- 構築: 時間 $O(1)$、領域 $O(1)$
- `empty`, `size`: 時間 $O(1)$、追加領域 $O(1)$

## `insert`

```cpp
bool insert(const PointType& point)
```

`point` を追加する。戻り値は凸包が変化したか。同一点の再追加や、現在の凸包の
内部・辺上にある点の追加では `false` を返す。

$k$ をこの追加で凸包から消える頂点数とする。

- 時間計算量: expected $O((k+1)\log(H+1))$
- 償却時間計算量: 追加列全体で1回あたり expected $O(\log(N+1))$
- 追加領域計算量: expected $O(\log(H+1))$（再帰stack）

ここで $H$ は追加直前の凸包頂点数、$N$ はそれまでに追加した点数。各点は各chain
から高々1回しか削除されない。

## 内外判定

```cpp
int contains(const PointType& point) const
```

点が凸包の外部なら `0`、境界上なら `1`、内部なら `2` を返す。空集合・一点・
線分に退化した凸包にも対応する。

- 時間計算量: expected $O(\log(H+1))$
- 追加領域計算量: $O(1)$

## 面積

```cpp
WideInteger doubled_area() const
```

凸包の面積の2倍を非負の任意精度整数で返す。

- 時間計算量: $O(1)$
- 追加領域計算量: 戻り値を除き $O(1)$

## 支持点

```cpp
std::optional<PointType> support(const PointType& direction) const
```

`point.x * direction.x + point.y * direction.y` を最大化する凸包頂点を返す。
複数ある場合は辞書順最小の頂点を返す。空なら `std::nullopt`。

- 時間計算量: expected $O(\log(H+1))$
- 追加領域計算量: $O(1)$

## 頂点列

```cpp
std::vector<PointType> vertices() const
```

凸包頂点を辞書順最小の頂点から反時計回りに返す。辺上の中間点と末尾の始点重複は
含めない。線分では辞書順に2端点を返す。

- 時間計算量: $O(H)$
- 追加領域計算量: 戻り値を含め $O(H)$

## 注意点

追加だけを扱い、削除は扱わない。`Coordinate` は整数型である必要がある。
treapの高さに関する計算量は内部乱数に対するexpected boundである。保存領域は
$O(H)$。座標差・外積・面積は任意精度で計算するため、符号付き整数overflowを
起こさない。
