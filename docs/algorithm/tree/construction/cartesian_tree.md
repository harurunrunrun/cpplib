---
title: Cartesian Tree (カーテシアン木)
documentation_of: ../../../../src/algorithm/tree/construction/cartesian_tree.hpp
---

列の添字の順序を中間順巡回の順序として保つ Cartesian Tree を構築する。
デフォルトの `std::less<T>` では min Cartesian Tree、`std::greater<T>` では max Cartesian Tree になる。

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
template<class T, class Compare = std::less<T>>
CartesianTreeResult cartesian_tree(
    const std::vector<T>& values,
    Compare compare = Compare());
```

`compare(a, b)` が真なら `a` を `b` より根に近くした Cartesian Tree を返す。
木の中間順巡回は必ず `0, 1, ..., N-1` になる。

比較上同値な要素、すなわち `compare(a, b)` と `compare(b, a)` がともに偽となる要素では、先に現れた添字を根に近くする。
同値な要素だけからなる列は右向きの鎖になる。

各要素は単調スタックに 1 度追加され、高々 1 度取り除かれる。
比較 1 回の計算量を $C$ とすると時間計算量は $O(NC + N)$ で、比較が $O(1)$ なら $O(N)$ である。
戻り値を含む空間計算量は $O(N)$ である。

## `build_cartesian_tree(values, compare)`

`cartesian_tree(values, compare)` の別名で、同じ結果・計算量を持つ。

## 使用例

```cpp
auto min_tree = cartesian_tree(a);
auto max_tree = cartesian_tree(a, std::greater<int>());
```

## 注意点

`Compare` は strict weak ordering を満たす必要がある。比較上同値な要素では左側の添字を優先し、空列の `root` は `-1` となる。
