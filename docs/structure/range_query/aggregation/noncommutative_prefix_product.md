---
title: Noncommutative Prefix Product (非可換累積積)
documentation_of: ../../../../src/structure/range_query/aggregation/noncommutative_prefix_product.hpp
---

非可換なモノイドの列を保持し、1 点の更新と、順序を保った接頭辞積・区間積を求める。

```cpp
template<auto Monoid, int MAX_SIZE = -1>
class NoncommutativePrefixProduct;
```

`Monoid` は既存の `Monoid<op, e>` などと同じく `op(left, right)` と `e()` を持つオブジェクトである。区間積は添字の小さい要素から順に右へ `op` を適用し、左右を入れ替えない。`MAX_SIZE` は要素数の上限であり、省略時（`-1`）は上限を設けず、実行時の要素数に合わせて `std::vector` で領域を確保する。

## コンストラクタ

```cpp
explicit NoncommutativePrefixProduct(int size = 0);
explicit NoncommutativePrefixProduct(const vector<value_type>& values);
```

長さ `size` の単位元の列、または `values` を初期列として構築する。

```cpp
using value_type = decay_t<decltype(Monoid.e())>;
```

## `capacity`, `size`, `empty`

```cpp
static constexpr int capacity() noexcept;  // MAX_SIZE を省略した場合は非 static で size() を返す
int size() const noexcept;
bool empty() const noexcept;
```

それぞれコンパイル時の最大要素数 `MAX_SIZE`、現在の要素数、列が空かどうかを返す。

## `get`, `set`

```cpp
const value_type& get(int index) const;
void set(int index, const value_type& value);
```

1 要素の取得と置き換えを行う。

## `prefix_product`

```cpp
value_type prefix_product(int right) const;
```

半開区間 `[0, right)` の積を返す。`right == 0` では単位元を返す。

## `range_product`

```cpp
value_type range_product(int left, int right) const;
```

半開区間 `[left, right)` の積を添字の順に計算して返す。空区間では単位元を返す。クエリ中は左から集める積と右から集める積を別々に持ち、最後に `op(left_product, right_product)` を返す。

## `all_product`

```cpp
value_type all_product() const;
```

列全体の積を返す。

## 時間計算量

$N$ を `size()` とし、$M$ を `MAX_SIZE`（省略時は $N$）とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 長さ指定のコンストラクタ | $O(M)$ | オブジェクト内に $O(M)$ |
| `vector` 指定のコンストラクタ | $O(M)$ | オブジェクト内に $O(M)$ |
| `capacity`, `size`, `empty`, `get`, `all_product` | $O(1)$ | $O(1)$ |
| `set`, `prefix_product`, `range_product` | $O(\log M)$ | $O(1)$ |

## 注意点

- コンストラクタの長さが負、または `MAX_SIZE` を超えると `std::runtime_error` を送出する。
- `get` と `set` の添字が `[0, size())` の外なら `std::runtime_error` を送出する。
- クエリの端点が `0 <= left <= right <= size()` を満たさなければ `std::runtime_error` を送出する。
- `op` は結合則を満たし、`e()` は左右の単位元でなければならない。可換性は不要である。
