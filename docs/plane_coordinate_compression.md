---
title: Plane Coordinate Compression (平面座標圧縮)
documentation_of: ../src/algorithm/other/utility/plane_coordinate_compression.hpp
---

平面上で別々に与えた x 座標と y 座標を昇順の連続indexへ圧縮する。格子の連結性や座標間の空白を残したい場合は、各入力値の直前・直後の整数も集合へ加えられる。

```cpp
template<class X, class Y, int MAX_X_SIZE, int MAX_Y_SIZE>
class PlaneCoordinateCompression;
```

`X` と `Y` は `bool` 以外の整数型である。`MAX_X_SIZE` と `MAX_Y_SIZE` は、隣接値を追加して重複を除いた後の各軸の最大要素数を表す。

```cpp
struct CoordinateNeighborPolicy {
    bool predecessor = false;
    bool successor = false;
};
```

隣接値はこのpolicyでpredecessorとsuccessorを独立に指定する。

## Constructor

```cpp
PlaneCoordinateCompression();

PlaneCoordinateCompression(
    const vector<X>& x_values,
    const vector<Y>& y_values,
    CoordinateNeighborPolicy x_neighbors = {},
    CoordinateNeighborPolicy y_neighbors = {}
);
```

default constructorは空の圧縮を作る。引数付きconstructorは `build` と同じ規則で両軸を構築する。

## `build`

```cpp
void build(
    const vector<X>& x_values,
    const vector<Y>& y_values,
    CoordinateNeighborPolicy x_neighbors = {},
    CoordinateNeighborPolicy y_neighbors = {}
);
```

各軸を独立にsortして重複を除く。対応するpolicyの `predecessor` が真なら入力値 `v` ごとに表現可能な `v - 1`、`successor` が真なら表現可能な `v + 1` も追加する。型の最小値に対するpredecessorと最大値に対するsuccessorは追加しない。両flagは軸ごとに独立して指定できる。

容量超過時に例外を送出した場合、既存の圧縮結果は変更しない。

## `x_size`, `y_size`, `size`, `empty`

```cpp
size_t x_size() const noexcept;
size_t y_size() const noexcept;
array<size_t, 2> size() const noexcept;
bool empty() const noexcept;
```

圧縮後の各軸の要素数を返す。`size()` は `{x_size(), y_size()}`、`empty()` は両軸が空のときに限り真を返す。

## `x_values`, `y_values`

```cpp
span<const X> x_values() const noexcept;
span<const Y> y_values() const noexcept;
```

圧縮後の昇順な座標列を参照するspanを返す。spanは次の `build` まで有効である。

## `contains_x`, `contains_y`

```cpp
bool contains_x(X value) const noexcept;
bool contains_y(Y value) const noexcept;
```

指定値が対応する圧縮集合に含まれるかを返す。

## `x_index`, `y_index`

```cpp
size_t x_index(X value) const;
size_t y_index(Y value) const;
```

指定値の0-indexed圧縮indexを返す。

## `x_value`, `y_value`

```cpp
const X& x_value(size_t index) const;
const Y& y_value(size_t index) const;
```

0-indexed圧縮indexに対応する元の座標値を返す。

## 時間計算量

入力列長をそれぞれ $N_x,N_y$、圧縮後の要素数を $K_x,K_y$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| constructor, `build` | $O(N_x\log N_x+N_y\log N_y)$ | $O(N_x+N_y)$ |
| `x_size`, `y_size`, `size`, `empty`, `x_values`, `y_values`, `x_value`, `y_value` | $O(1)$ | $O(1)$ |
| `contains_x`, `x_index` | $O(\log K_x)$ | $O(1)$ |
| `contains_y`, `y_index` | $O(\log K_y)$ | $O(1)$ |

neighbor追加時も候補数は入力列長の高々3倍なので、漸近計算量は変わらない。

## 注意点

- `x_index` または `y_index` に圧縮集合にない値を渡すと `runtime_error` を送出する。lower boundのindexを得るAPIではない。
- `x_value` または `y_value` に対応する軸のsize以上のindexを渡すと `runtime_error` を送出する。
- 圧縮後の要素数がtemplate容量を超えると `runtime_error` を送出する。
- neighbor追加は整数の1だけ離れた値を加える。座標差そのものを保存する圧縮ではない。
- `x_values()` と `y_values()` が返したspanは、同じobjectへの次の `build` で無効になる。
