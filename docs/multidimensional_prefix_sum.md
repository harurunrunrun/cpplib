---
title: Multidimensional Prefix Sum (多次元累積和)
documentation_of: ../src/algorithm/other/offline_query/multidimensional_prefix_sum.hpp
---

静的な直方体配列をD次元累積和へ変換し、半開直方体の総和を求める。

```cpp
template<class T, size_t Dimension, size_t MAX_STORAGE_SIZE>
class MultidimensionalPrefixSum;
```

座標とshapeには `array<size_t, Dimension>` を使う。内部では各軸に長さ1の零境界を加えるため、`MAX_STORAGE_SIZE` は $\prod_i(\mathrm{shape}_i+1)$ 以上必要である。

`T` はdefault constructor、`+=`、`-=`を持つ加法群として扱う。

## Constructor

```cpp
explicit MultidimensionalPrefixSum(const coordinate_type& shape);
```

指定shapeの全要素を `T{}` で初期化する。`coordinate_type` は次のaliasである。

```cpp
using coordinate_type = array<size_t, Dimension>;
```

## `shape`, `size`, `empty`, `built`, `storage_size`, `capacity`

```cpp
const coordinate_type& shape() const noexcept;
size_t size() const noexcept;
bool empty() const noexcept;
bool built() const noexcept;
size_t storage_size() const noexcept;
static constexpr size_t capacity() noexcept;
```

- `shape()` は各軸の長さを返す。
- `size()` は元の配列の要素数 $\prod_i\mathrm{shape}_i$ を返す。
- `empty()` はいずれかの軸長が0のとき真を返す。
- `built()` は `build()` 完了後に真を返す。
- `storage_size()` は零境界を含む使用領域 $\prod_i(\mathrm{shape}_i+1)$ を返す。
- `capacity()` はcompile時容量 `MAX_STORAGE_SIZE` を返す。

## `set`, `add`

```cpp
void set(const coordinate_type& coordinate, const T& value);
void add(const coordinate_type& coordinate, const T& value);
```

`build()` 前の1要素を代入または加算する。各座標成分は対応するshape未満でなければならない。

## `build`

```cpp
void build();
```

現在の要素列をD次元累積和へ変換する。完了後は `set`、`add`、再度の `build` を使用できない。

## `get`

```cpp
T get(const coordinate_type& coordinate) const;
```

指定した1要素を返す。`build()` の前後で使用できる。

## `prefix_sum`

```cpp
T prefix_sum(const coordinate_type& end) const;
```

$0\leq p_i<\mathrm{end}_i$ を全軸で満たす要素の総和を返す。各 `end[i]` は `shape()[i]` 以下でよい。

## `range_sum`

```cpp
T range_sum(
    const coordinate_type& begin,
    const coordinate_type& end
) const;
```

各軸について `begin[i] <= p[i] < end[i]` を満たす半開直方体の総和を返す。空区間の総和は `T{}` である。

## 時間計算量

$D=\mathrm{Dimension}$、$N=\prod_i(\mathrm{shape}_i+1)$、$C=\mathrm{MAX\_STORAGE\_SIZE}$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| constructor | $O(C)$ | object内に $O(C)$ |
| `shape`, `size`, `empty`, `built`, `storage_size`, `capacity` | $O(1)$ | $O(1)$ |
| `set`, `add`, build前の `get`, `prefix_sum` | $O(D)$ | $O(1)$ |
| `build` | $O(DN)$ | $O(1)$ |
| build後の `get`, `range_sum` | $O(D+2^D)$ | $O(1)$ |

## 注意点

- constructorは `shape[i] + 1`、その積、または内部容量にoverflow・超過がある場合に `runtime_error` を送出する。
- `set`、`add`、`get` の座標がshape外なら `runtime_error` を送出する。
- `prefix_sum` と `range_sum` は `build()` 後だけ使用できる。違反時は `runtime_error` を送出する。
- `set`、`add`、再度の `build` を `build()` 後に呼ぶと `runtime_error` を送出する。
- `range_sum` で `begin[i] > end[i]`、またはcornerがshape外なら `runtime_error` を送出する。
- `Dimension` は正で、`size_t` のbit数未満でなければならない。queryは $2^D$ 個のcornerを列挙するため、Dimensionが大きい用途には適さない。
