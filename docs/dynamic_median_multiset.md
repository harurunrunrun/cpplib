---
title: Dynamic Median Multiset (動的中央値multiset) [dynamic_median_multiset]
documentation_of: ../src/structure/other/dynamic_median_multiset.hpp
---

要素の挿入・削除を行いながら、順序の中央にある要素を取得するmultiset。
重複する値を個別に保持する。

## テンプレート引数

~~~cpp
DynamicMedianMultiset<T, Compare>
~~~

- 値の型 `T`
- 値の順序を定める比較関数 `Compare`

中央値は `Compare` が定める順序で数える。

## 構築

~~~cpp
explicit DynamicMedianMultiset(const Compare& compare = Compare())
~~~

空のmultisetを構築する。

## 要素数

~~~cpp
size_t size() const
bool empty() const
void clear()
~~~

`size` は重複を含む要素数、`empty` は空かを返す。
`clear` は全要素を削除する。

## 更新・検索

~~~cpp
void insert(T value)
bool contains(const T& value) const
size_t count(const T& value) const
bool erase_one(const T& value)
size_t erase_all(const T& value)
~~~

- `insert`: 値を1個追加する。
- `contains`: `Compare` に関して同値な要素が存在するかを返す。
- `count`: 同値な要素の個数を返す。
- `erase_one`: 同値な要素を1個削除し、削除できたかを返す。
- `erase_all`: 同値な要素をすべて削除し、削除数を返す。

## 中央値

~~~cpp
const T& lower_median() const
const T& upper_median() const
const T& odd_median() const
pair<T, T> even_medians() const
~~~

要素を `Compare` の順に並べ、要素数を $N$ とする。

- `lower_median`: 添字 $(N-1)/2$ の要素を返す。
- `upper_median`: 添字 $N/2$ の要素を返す。
- `odd_median`: $N$ が奇数のとき中央の要素を返す。
- `even_medians`: $N$ が正の偶数のとき、2個の中央要素をpairで返す。

参照を返すAPIの参照は、次の非const操作まで有効である。

## 時間計算量

$N$ を操作前の要素数とし、比較を $O(1)$ とする。

- constructor, `size`, `empty`: $O(1)$
- `clear`: $O(N)$
- `insert`, `erase_one`: $O(\log(N+1))$
- `contains`: $O(\log(N+1))$
- `count`, `erase_all`: $O(\log(N+1)+K)$、$K$ は同値な要素数
- `lower_median`, `upper_median`, `odd_median`, `even_medians`: $O(1)$

## 空間計算量

- object全体: $O(N)$
- 各更新操作の追加領域: $O(1)$

## 注意点

- `lower_median`, `upper_median`, `odd_median`, `even_medians` は空のとき `std::out_of_range` を送出する。
- `odd_median` は要素数が偶数のとき `std::logic_error` を送出する。
- `even_medians` は要素数が奇数のとき `std::logic_error` を送出する。
- `Compare` はstrict weak orderingを満たす必要がある。
