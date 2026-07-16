---
title: Point Add Rectangle Affine Rectangle Sum (点加算・長方形アフィン作用・長方形和)
documentation_of: ../src/structure/other/point_add_rectangle_affine_rectangle_sum.hpp
---

2次元上の疎な点集合について、点の値への加算、半開長方形内の全点へのアフィン作用、長方形内の値の総和を処理する。
平衡KD木の部分木に遅延作用を保持し、新規座標は平方根個程度のbufferへ置いて定期的に再構築する。

## テンプレート引数

```cpp
PointAddRectangleAffineRectangleSum<T>
```

`T` は `T{}`、`T{1}`、加算、乗算、コピー、代入を利用できる値型である。
座標型は `int64_t` である。

## `Point`

```cpp
struct Point {
    int64_t x;
    int64_t y;
    T value;
};
```

初期構築用の重み付き点を表す。同じ座標が複数あれば値を加算して1点へまとめる。

## コンストラクタ

```cpp
PointAddRectangleAffineRectangleSum();
PointAddRectangleAffineRectangleSum(const vector<Point>& initial_points);
```

空の構造、または初期点集合から構築する。

## `point_add`

```cpp
bool point_add(int64_t x, int64_t y, const T& delta);
```

座標 `(x,y)` の値へ `delta` を加える。座標が新規なら `true`、既存なら `false` を返す。
新規点には過去の長方形作用を適用しない。

## `rectangle_affine`

```cpp
void rectangle_affine(
    int64_t x_left,
    int64_t x_right,
    int64_t y_lower,
    int64_t y_upper,
    const T& multiplier,
    const T& addend
);
```

半開長方形 `[x_left,x_right) x [y_lower,y_upper)` に現在存在する各点の値 `value` を
`multiplier * value + addend` へ置き換える。

作用 `f(value)=a*value+b` の後に `g(value)=c*value+d` を呼ぶと、合成は
`g(f(value))=(c*a)*value+(c*b+d)` の順である。

## `rectangle_sum`

```cpp
T rectangle_sum(x_left, x_right, y_lower, y_upper);
```

半開長方形内に現在存在する全点の値の総和を返す。

## `size` / `clear`

```cpp
size_t size() const;
void clear();
```

異なる座標数を返す、または全点を消去する。

## 使用例

```cpp
PointAddRectangleAffineRectangleSum<long long> points;
points.point_add(-3, 5, 10);
points.rectangle_affine(-10, 0, 0, 8, 2, 1);
assert(points.rectangle_sum(-4, -2, 5, 6) == 21);
```

## API別の時間計算量・空間計算量

異なる座標数を $N$ とする。KD木の再構築と再構築の間ではbufferを $O(\sqrt N)$ に保つ。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 空コンストラクタ、`size` | $O(1)$ | $O(1)$ |
| 初期点コンストラクタ | $O(N\log N)$ | $O(N)$ |
| 既存座標への `point_add` | $O(\sqrt N)$ | $O(1)$ |
| 新規座標への `point_add` | 償却 $O(\sqrt N\log N)$、再構築時worst-case $O(N\log N)$ | 償却 $O(1)$ |
| `rectangle_affine`, `rectangle_sum` | $O(\sqrt N)$ | $O(\log N)$ |
| `clear` | $O(N)$ | $O(1)$ |

全体の保存領域は $O(N)$ である。

## 注意点

- 長方形端点は任意の `int64_t` を使える。逆順なら `invalid_argument` を送出する。
- 幅または高さ0の長方形は空で、作用せず和は0となる。
- 値、アフィン係数、部分木和の全中間結果が `T` で表現できる必要がある。
- `rectangle_sum` は遅延作用を子へ伝播するため非 `const` である。
