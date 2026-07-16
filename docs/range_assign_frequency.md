---
title: Range Assign Frequency (区間代入・全体頻度)
documentation_of: ../src/structure/other/range_assign_frequency.hpp
---

数列を同じ値の極大連続区間に分けて管理する。半開区間への一括代入と、指定した値が
数列全体に何個あるかの取得を行える。値ごとの頻度を別の平衡二分木に持つため、頻度を
得るために全区間を走査しない。

## `RangeAssignFrequency`

```cpp
template<class T, class Compare = std::less<T>>
class RangeAssignFrequency;
```

`Compare` によって同値な値を同一値として扱う。

## コンストラクタ

```cpp
RangeAssignFrequency(Compare compare = Compare{});
RangeAssignFrequency(std::size_t size, const T& initial_value,
                     Compare compare = Compare{});
RangeAssignFrequency(const std::vector<T>& values,
                     Compare compare = Compare{});
```

空列、同じ初期値を持つ長さ `size` の列、または `values` から構築する。

- 時間計算量: 空列は $O(1)$、一様列は $O(\log U)$、`values` は $O(N\log U)$
- 空間計算量: $O(R+U)$

$R$ は連続区間数、$U$ は異なる値の数とする。

## `size` / `empty` / `segment_count`

```cpp
std::size_t size() const;
bool empty() const;
std::size_t segment_count() const;
```

要素数、空かどうか、同じ値の極大連続区間数を返す。

- 時間計算量: $O(1)$

## `get`

```cpp
const T& get(std::size_t index) const;
```

`a[index]` を返す。

- 時間計算量: $O(\log R)$

## `frequency`

```cpp
std::size_t frequency(const T& value) const;
```

列全体で `value` と同値な要素の個数を返す。存在しなければ0。

- 時間計算量: $O(\log U)$

## `segment`

```cpp
struct Segment { std::size_t left, right; T value; };
Segment segment(std::size_t index) const;
```

`index` を含む極大区間 `[left, right)` とその値を返す。

- 時間計算量: $O(\log R)$

## `segments`

```cpp
std::vector<Segment> segments() const;
```

全極大区間を左から順に返す。

- 時間計算量: $O(R)$
- 追加空間計算量: $O(R)$

## `assign` / `set`

```cpp
void assign(std::size_t left, std::size_t right, const T& value);
void set(std::size_t left, std::size_t right, const T& value);
```

全ての `left <= i < right` に対して `a[i] = value` とする。`set` は別名。
更新で消える極大区間数を $K$ とすると:

- 時間計算量: $O((K+1)(\log R+\log U))$
- 追加空間計算量: $O(1)$

各更新が新しく作る区間境界は高々2個なので、初期区間数を $R_0$、更新回数を $Q$ と
した全更新の合計は $O((R_0+Q)\log(N+Q))$ である。

## 注意点

添字は0始まりで、有効範囲は `0 <= left <= right <= size()`。
範囲外の `get`、`segment`、`assign` は `runtime_error` を送出する。
`T` はコピー可能で、`Compare` がstrict weak orderingを満たす必要がある。
