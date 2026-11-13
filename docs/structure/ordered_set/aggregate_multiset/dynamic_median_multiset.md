---
title: Dynamic Median Multiset (動的中央値multiset)
documentation_of: ../../../../src/structure/ordered_set/aggregate_multiset/dynamic_median_multiset.hpp
---

要素の挿入・削除を行いながら、順序の中央にある要素を取得する多重集合。重複する値は個別に保持する。
内部では、下半分と上半分を 2 つの `std::multiset` に分けて持つ。

## テンプレート引数

```cpp
DynamicMedianMultiset<T, Compare = std::less<T>>
```

- `T`: 値の型
- `Compare`: 値の順序を定める比較関数

中央値は `Compare` が定める順序で数える。

## コンストラクタ

```cpp
explicit DynamicMedianMultiset(const Compare& compare = Compare())
```

空の多重集合を構築する。

## 要素数

```cpp
size_t size() const
bool empty() const
void clear()
```

`size` は重複を含む要素数を、`empty` は空かどうかを返す。`clear` は全要素を削除する。

## 更新・検索

```cpp
void insert(T value)
bool contains(const T& value) const
size_t count(const T& value) const
bool erase_one(const T& value)
size_t erase_all(const T& value)
```

- `insert`: 値を 1 個追加する。
- `contains`: `Compare` について同値な要素が存在するかを返す。
- `count`: 同値な要素の個数を返す。
- `erase_one`: 同値な要素を 1 個削除し、削除できたかを返す。
- `erase_all`: 同値な要素をすべて削除し、削除した個数を返す。

## 中央値

```cpp
const T& lower_median() const
const T& upper_median() const
const T& odd_median() const
pair<T, T> even_medians() const
```

要素を `Compare` の順に並べ、要素数を $N$ とする（添字は 0 始まり）。

- `lower_median`: 添字 $\lfloor (N-1)/2 \rfloor$ の要素を返す。
- `upper_median`: 添字 $\lfloor N/2 \rfloor$ の要素を返す。
- `odd_median`: $N$ が奇数のとき、中央の要素を返す。
- `even_medians`: $N$ が正の偶数のとき、中央の 2 個の要素を `pair` で返す。

参照を返す関数の参照は、次の非 `const` 操作まで有効である。

## 時間計算量

$N$ を操作前の要素数とし、比較を $O(1)$ とする。

- コンストラクタ、`size`、`empty`: $O(1)$
- `clear`: $O(N)$
- `insert`、`erase_one`、`contains`: $O(\log(N+1))$
- `count`: 同値な要素の個数を $K$ として $O(\log(N+1)+K)$
- `erase_all`: 削除後に最大 $K$ 個の要素を左右の間で移し直すため、$O((K+1)\log(N+1))$
- `lower_median`、`upper_median`、`odd_median`、`even_medians`: $O(1)$

## 空間計算量

- オブジェクト全体: $O(N)$
- 各更新操作の追加領域: $O(1)$

## 注意点

- `lower_median`、`upper_median`、`odd_median`、`even_medians` は、空のとき `std::out_of_range` を送出する。
- `odd_median` は要素数が偶数のとき、`even_medians` は要素数が奇数のとき `std::logic_error` を送出する。
- `Compare` は狭義の弱順序を満たす必要がある。
