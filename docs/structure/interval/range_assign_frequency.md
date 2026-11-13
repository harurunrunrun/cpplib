---
title: Range Assign Frequency (区間代入・全体頻度)
documentation_of: ../../../src/structure/interval/range_assign_frequency.hpp
---

数列を、同じ値が続く極大な連続区間に分けて管理する。半開区間への一括代入と、指定した値が数列全体に何個あるかの取得を行う。
値ごとの頻度を別の平衡二分木（`std::map`）に持つため、頻度を得るために全区間を走査しない。

## RangeAssignFrequency

```cpp
template<class T, class Compare = std::less<T>>
class RangeAssignFrequency;
```

`Compare` について同値な値を、同じ値として扱う。

## コンストラクタ

```cpp
explicit RangeAssignFrequency(Compare compare = Compare{});
RangeAssignFrequency(std::size_t size, const T& initial_value,
                     Compare compare = Compare{});
explicit RangeAssignFrequency(const std::vector<T>& values,
                              Compare compare = Compare{});
```

空列、同じ初期値を持つ長さ `size` の列、または `values` から構築する。

## size / empty / segment_count

```cpp
std::size_t size() const;
bool empty() const;
std::size_t segment_count() const;
```

要素数、空かどうか、同じ値が続く極大な連続区間の個数を返す。

## get

```cpp
const T& get(std::size_t index) const;
```

$a_{\mathrm{index}}$ を返す。

## frequency

```cpp
std::size_t frequency(const T& value) const;
```

列全体で `value` と同値な要素の個数を返す。存在しなければ $0$ を返す。

## segment

```cpp
struct Segment { std::size_t left, right; T value; };
Segment segment(std::size_t index) const;
```

`index` を含む極大区間 $[\mathrm{left}, \mathrm{right})$ と、その値を返す。

## segments

```cpp
std::vector<Segment> segments() const;
```

すべての極大区間を左から順に返す。

## assign / set

```cpp
void assign(std::size_t left, std::size_t right, const T& value);
void set(std::size_t left, std::size_t right, const T& value);
```

すべての $\mathrm{left} \le i \lt \mathrm{right}$ に対して $a_i \gets \mathrm{value}$ とする。`set` は `assign` の別名である。

## 時間計算量

$N$ を要素数、$R$ を連続区間の個数、$U$ を異なる値の個数とする。

- コンストラクタ: 空列と一様な列は $O(1)$、`values` からは $O(N(\log U + \log R))$
- `size`、`empty`、`segment_count`: $O(1)$
- `get`、`segment`: $O(\log R)$
- `frequency`: $O(\log U)$
- `segments`: $O(R)$（追加領域 $O(R)$）
- `assign`、`set`: 更新で消える極大区間の個数を $K$ として $O((K+1)(\log R+\log U))$（追加領域 $O(1)$）

各更新が新しく作る区間の境界は高々 2 個なので、初期の区間数を $R_0$、更新回数を $Q$ とすると、全更新の合計は $O((R_0+Q)\log(N+Q))$ である。

## 空間計算量

$O(R+U)$。

## 注意点

- 添字は 0 始まりで、区間の有効範囲は $0 \le \mathrm{left} \le \mathrm{right} \le \mathrm{size}()$ である。
- 範囲外の `get`、`segment`、`assign` は `std::runtime_error` を送出する。
- `T` はコピー可能で、`Compare` は狭義の弱順序を満たす必要がある。
