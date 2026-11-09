---
title: Multidimensional Prefix Sum (多次元累積和)
documentation_of: ../../../../../src/algorithm/other/offline_query/grid_aggregation/multidimensional_prefix_sum.hpp
---

静的な $D$ 次元の直方体配列を $D$ 次元累積和へ変換し、半開直方体の総和を求める。

```cpp
template<class T, size_t Dimension, size_t MAX_STORAGE_SIZE = size_t(-1)>
class MultidimensionalPrefixSum;
```

座標と形状には `array<size_t, Dimension>` を使う。内部では各軸に長さ 1 の零境界を加えるため、固定長版では `MAX_STORAGE_SIZE` が $\prod_i(\mathrm{shape}_i+1)$ 以上でなければならない。

`MAX_STORAGE_SIZE` を省略すると、実行時サイズに合わせた `std::vector` を使う可変長版になる。次元数は演算の定義なのでテンプレート引数に残る。

```cpp
MultidimensionalPrefixSum<long long, 2> dynamic({rows, columns});
MultidimensionalPrefixSum<long long, 2, 1000000> fixed({rows, columns});
```

`T` はデフォルトコンストラクタ、`+=`、`-=` を持つ加法群として扱う。

## コンストラクタ

```cpp
explicit MultidimensionalPrefixSum(const coordinate_type& shape);
```

指定した形状の全要素を `T{}` で初期化する。`coordinate_type` は次の別名である。

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
- `empty()` はいずれかの軸長が $0$ のとき真を返す。
- `built()` は `build()` 完了後に真を返す。
- `storage_size()` は零境界を含む使用領域 $\prod_i(\mathrm{shape}_i+1)$ を返す。
- `capacity()` はコンパイル時の容量 `MAX_STORAGE_SIZE` を返す。

## `set`, `add`

```cpp
void set(const coordinate_type& coordinate, const T& value);
void add(const coordinate_type& coordinate, const T& value);
```

`build()` 前に 1 要素へ代入または加算する。各座標成分は対応する軸長未満でなければならない。

## `build`

```cpp
void build();
```

現在の要素列を $D$ 次元累積和へ変換する。完了後は `set`、`add`、再度の `build` を使用できない。

## `get`

```cpp
T get(const coordinate_type& coordinate) const;
```

指定した 1 要素を返す。`build()` の前後どちらでも使用できる（`build()` 後は大きさ 1 の `range_sum` として計算する）。

## `prefix_sum`

```cpp
T prefix_sum(const coordinate_type& end) const;
```

全軸で $0\leq p_i<\mathrm{end}_i$ を満たす要素の総和を返す。各 `end[i]` は `shape()[i]` 以下であればよい。

## `range_sum`

```cpp
T range_sum(
    const coordinate_type& begin,
    const coordinate_type& end
) const;
```

各軸について `begin[i] <= p[i] < end[i]` を満たす半開直方体の総和を返す。空区間の総和は `T{}` である。

## 計算量

$D$ を `Dimension`、$N=\prod_i(\mathrm{shape}_i+1)$、$C$ を `MAX_STORAGE_SIZE` とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| コンストラクタ | 固定長版 $O(C)$、可変長版 $O(N)$ | オブジェクト内に固定長版 $O(C)$、可変長版 $O(N)$ |
| `shape`, `size`, `empty`, `built`, `storage_size`, `capacity` | $O(1)$ | $O(1)$ |
| `set`, `add`, `build` 前の `get`, `prefix_sum` | $O(D)$ | $O(1)$ |
| `build` | $O(DN)$ | $O(1)$ |
| `build` 後の `get`, `range_sum` | $O(D+2^D)$ | $O(1)$ |

## 注意点

- コンストラクタは、`shape[i] + 1`、その積、または要素数の積がオーバーフローする場合と、使用領域が `MAX_STORAGE_SIZE` を超える場合に `runtime_error` を送出する。
- `set`、`add`、`get` の座標が形状の外なら `runtime_error` を送出する。
- `prefix_sum` と `range_sum` は `build()` 後にだけ使用できる。違反時は `runtime_error` を送出する。
- `build()` 後に `set`、`add`、再度の `build` を呼ぶと `runtime_error` を送出する。
- `range_sum` で `begin[i] > end[i]` の場合、または角の座標が形状の外の場合は `runtime_error` を送出する。
- `Dimension` は正で、`size_t` のビット数未満でなければならない（`static_assert`）。クエリは $2^D$ 個の角を列挙するため、次元数が大きい用途には適さない。
