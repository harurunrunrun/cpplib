---
title: Cartesian Tree Parent Array (カーテシアン木親配列) [cartesian_tree]
documentation_of: ../../../../src/algorithm/tree/construction/cartesian_tree_parent_array.hpp
---

Cartesian Tree の各頂点について、親の頂点番号を返す。

## API

```cpp
template<class T, class Compare = std::less<T>>
std::vector<int> cartesian_tree_parent_array(
    const std::vector<T>& values,
    Compare compare = Compare()
)
```

`compare(a, b)` が真なら `a` を `b` より根に近くした Cartesian Tree を構築し、各添字の親を返す。
根の親は根自身とする。空列には空列を返す。
比較上同値な値では、小さい添字を根に近くする。

## 時間計算量

比較 1 回の計算量を $C$、$N$ を `values.size()` として $O(NC+N)$ である。
比較が $O(1)$ なら $O(N)$ である。

## 空間計算量

Cartesian Tree 全体の一時的な結果と戻り値を含めて $O(N)$ である。

## 注意点

- `Compare` は strict weak ordering を満たさなければならない。
- 入力と戻り値の添字は 0-indexed である。
