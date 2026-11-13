---
title: Fractional Cascading (フラクショナルカスケーディング)
documentation_of: ../../../../src/structure/range_query/value_query/fractional_cascading.hpp
---

複数の整列済みの列に対して同じ値で行う二分探索を、1 回の二分探索と層の間のリンクで処理する。

## API

```cpp
template<class T>
class FractionalCascading;
```

- `FractionalCascading()`: 空の構造を作る。
- `explicit FractionalCascading(vector<vector<T>> catalogs)`: 整列済みの列の集まりを前処理する。
- `catalog_count()`: 列の個数を返す。
- `lower_bounds(value)`: 各列における `value` の `lower_bound` の添字を並べた `vector<int>` を返す。

## 時間計算量

$S$ を全要素数、$K$ を列の個数とする。

- 構築: $O(S\log S)$
- `lower_bounds`: $O(\log S+K)$
- `catalog_count`: $O(1)$

空間計算量は $O(S)$ である。

## 注意点

- 各入力列は昇順に整列されていなければならない。整列されていない列があると `std::invalid_argument` を送出する。
- `T` には `operator<` と `operator==` が必要である。
