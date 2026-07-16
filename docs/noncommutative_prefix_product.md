---
title: Noncommutative Prefix Product (非可換累積積)
documentation_of: ../src/structure/other/noncommutative_prefix_product.hpp
---

非可換Monoidの列を保持し、point setと順序を保ったprefix・range積を求める。

```cpp
template<auto Monoid, int MAX_SIZE>
class NoncommutativePrefixProduct;
```

`Monoid` は既存の `Monoid<op, e>` などと同じく `op(left, right)` と `e()` を持つobjectである。range積はindexの小さい要素から右へ `op` を適用し、左右を交換しない。

## Constructor

```cpp
explicit NoncommutativePrefixProduct(int size = 0);
explicit NoncommutativePrefixProduct(const vector<value_type>& values);
```

長さ `size` の単位元列、または `values` を初期列として構築する。

```cpp
using value_type = decay_t<decltype(Monoid.e())>;
```

## `capacity`, `size`, `empty`

```cpp
static constexpr int capacity() noexcept;
int size() const noexcept;
bool empty() const noexcept;
```

compile時最大要素数、現在の要素数、列が空かを返す。

## `get`, `set`

```cpp
const value_type& get(int index) const;
void set(int index, const value_type& value);
```

1要素の取得と置換を行う。

## `prefix_product`

```cpp
value_type prefix_product(int right) const;
```

半開区間 `[0, right)` の積を返す。`right == 0` では単位元を返す。

## `range_product`

```cpp
value_type range_product(int left, int right) const;
```

半開区間 `[left, right)` の積をindex順に返す。空区間では単位元を返す。query中は左から集める積と右から集める積を分け、最後に `op(left_product, right_product)` とする。

## `all_product`

```cpp
value_type all_product() const;
```

列全体の積を返す。

## 時間計算量

$N=\mathrm{size}$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| size指定constructor | $O(\mathrm{MAX\_SIZE})$ | object内に $O(\mathrm{MAX\_SIZE})$ |
| vector指定constructor | $O(\mathrm{MAX\_SIZE})$ | object内に $O(\mathrm{MAX\_SIZE})$ |
| `capacity`, `size`, `empty`, `get`, `all_product` | $O(1)$ | $O(1)$ |
| `set`, `prefix_product`, `range_product` | $O(\log N)$ | $O(1)$ |

## 注意点

- constructorの長さが負、または `MAX_SIZE` を超えると `runtime_error` を送出する。
- `get` と `set` のindexが `[0, size())` 外なら `runtime_error` を送出する。
- queryの端点が `0 <= left <= right <= size()` を満たさなければ `runtime_error` を送出する。
- `op` は結合則を満たし、`e()` は左右の単位元でなければならない。可換性は不要である。
