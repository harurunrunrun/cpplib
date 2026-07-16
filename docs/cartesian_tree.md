---
title: Cartesian Tree (カーテシアン木)
documentation_of: ../src/algorithm/tree/cartesian_tree.hpp
---

## Cartesian Tree

列の添字順を中間順巡回の順序として保つ Cartesian Tree を構築する。
既定の `std::less<T>` では min Cartesian Tree、`std::greater<T>` では max Cartesian Tree になる。

## `CartesianTreeResult`

- `int root`: 根の添字。空列では `-1`。
- `std::vector<int> parent`: 各頂点の親。根では `-1`。
- `std::vector<int> left`: 各頂点の左の子。存在しなければ `-1`。
- `std::vector<int> right`: 各頂点の右の子。存在しなければ `-1`。
- `int size() const`: 頂点数を返す。$O(1)$。
- `bool empty() const`: 空かを返す。$O(1)$。

結果のコピーは $O(N)$、ムーブは `std::vector` のムーブと同じ計算量である。

## `cartesian_tree(values, compare)`

```cpp
template <class T, class Compare = std::less<T>>
CartesianTreeResult cartesian_tree(
    const std::vector<T>& values,
    Compare compare = Compare());
```

`compare(a, b)` が真なら `a` を `b` より根に近くする Cartesian Tree を返す。
木の中間順巡回は必ず `0, 1, ..., N-1` になる。

比較上同値、すなわち `compare(a, b)` と `compare(b, a)` がともに偽の要素では、
先に現れた添字を根に近くする。同値な要素だけの列は右向きの鎖になる。
`Compare` は strict weak ordering を満たす必要がある。

各要素は単調スタックへ一度追加され、一度以下だけ取り除かれる。
比較一回の計算量を $C$ とすると時間計算量は $O(NC + N)$、
比較が $O(1)$ なら $O(N)$、返り値を含む空間計算量は $O(N)$。

## `build_cartesian_tree(values, compare)`

`cartesian_tree(values, compare)` の別名。同じ結果・計算量を持つ。

## 使用例

```cpp
auto min_tree = cartesian_tree(a);
auto max_tree = cartesian_tree(a, std::greater<int>());
```

## 注意点

`Compare` はstrict weak orderingを満たす必要がある。比較上同値な要素では左側のindexを優先し、空列のrootは-1となる。
