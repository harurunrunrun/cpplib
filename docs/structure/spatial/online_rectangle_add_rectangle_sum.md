---
title: Online Rectangle Add Rectangle Sum (オンライン長方形加算・長方形和)
documentation_of: ../../../src/structure/spatial/online_rectangle_add_rectangle_sum.hpp
---

座標を事前に圧縮せず、軸平行な半開長方形への一様加算と長方形和をオンラインで処理する。
長方形差分の 4 隅を疎な二次元 Fenwick 木に格納するため、巨大な座標範囲でも更新された節点だけを確保する。

## 座標と値の意味

すべての端点は `int64_t` の整数で、長方形は `[x_left, x_right) × [y_lower, y_upper)` である。
次の 2 つの解釈は、整数端点では同じ値を返す。

- 格子セル: 各整数セル `[x, x + 1) × [y, y + 1)` が持つ値の総和
- 連続平面: 長方形内で一定な密度の面積積分

点だけに値を置く構造ではない。幅または高さが 0 の長方形に対する更新は何もせず、和の取得は `T{}` を返す。

## テンプレート引数

```cpp
OnlineRectangleAddRectangleSum<T>
```

`T` は `T{}`、加減算、`int64_t` との乗算、コピーと代入を利用できる値型である。

## コンストラクタ

```cpp
OnlineRectangleAddRectangleSum<T> structure(coordinate_low, coordinate_high);
```

x 軸と y 軸の両方で、共通に閉区間 `[coordinate_low, coordinate_high]` の端点を許す。
更新されるセルの座標範囲は `[coordinate_low, coordinate_high)` である。
座標幅 `coordinate_high - coordinate_low` は正かつ `INT64_MAX` 以下でなければならず、そうでなければ `invalid_argument` を送出する。

## `add_rectangle`

```cpp
void add_rectangle(x_left, x_right, y_lower, y_upper, const T& value);
```

指定した半開長方形の全セル（連続平面として見れば密度）に `value` を加える。
座標の事前登録を必要とせず、更新直後から和の取得に反映される。

## `rectangle_sum`

```cpp
T rectangle_sum(x_left, x_right, y_lower, y_upper) const;
```

指定した半開長方形の格子セルの値の和（連続平面として見れば密度の面積積分）を返す。

## 管理用 API

```cpp
int64_t coordinate_low() const;
int64_t coordinate_high() const;
void reserve(size_t expected_fenwick_cells);
void clear();
size_t stored_node_count() const;
```

- `coordinate_low`, `coordinate_high` は構築時の境界を返す。
- `reserve` は疎 Fenwick 木のハッシュテーブル領域を予約する。
- `clear` は全更新を消去するが、座標境界は維持する。
- `stored_node_count` は現在確保している疎 Fenwick 木のセル数を返す。

## 使用例

```cpp
OnlineRectangleAddRectangleSum<long long> rectangles(-1000000000000LL, 1001);
rectangles.add_rectangle(-5, 3, 10, 20, 4);
auto sum = rectangles.rectangle_sum(-2, 2, 12, 15);
```

## API 別の時間計算量・空間計算量

座標幅を $C$、空でない更新の回数を $U$、現在の疎 Fenwick 木のセル数を $K$ とする。ハッシュテーブル操作は期待 $O(1)$ とする。

| API | 期待時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(1)$ | $O(1)$ |
| `add_rectangle` | $O(\log^2 C)$ | 追加で $O(\log^2 C)$ |
| `rectangle_sum` | $O(\log^2 C)$ | $O(1)$ |
| `reserve` | ならし $O(K)$ | $O(K)$ |
| `clear` | $O(K)$ | $O(1)$ |
| 境界の取得、`stored_node_count` | $O(1)$ | $O(1)$ |

全体の保存領域は $O(U\log^2 C)$、実際の領域は $O(K)$ である。

## 注意点

- 長方形の全端点は構築時の閉区間内になければならない。範囲外なら `out_of_range`、`x_right < x_left` または `y_upper < y_lower` なら `invalid_argument` を送出する。
- 内部の多項式係数は `coordinate_low` を原点として計算する。値・座標差・面積を掛けた全中間値が `T` で表現できる必要がある。
- `unordered_map` を使うため、操作時間の保証は期待計算量である。
