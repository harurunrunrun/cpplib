---
title: Cartesian Tree Parent Array (カーテシアン木親配列) [cartesian_tree]
documentation_of: ../src/algorithm/tree/cartesian_tree_parent_array.hpp
---

Cartesian Treeの各頂点について親頂点番号を返す。

## cartesian_tree_parent_array

```cpp
template<class T, class Compare = less<T>>
vector<int> cartesian_tree_parent_array(
    const vector<T>& values,
    Compare compare = Compare()
)
```

`compare(a, b)` が真なら `a` を `b` より根に近くするCartesian Treeを構築し、
各indexの親を返す。根の親は根自身とする。空列には空列を返す。
比較上同値な値では小さいindexを根に近くする。

## 時間計算量

比較1回を $C$、`N = values.size()` として $O(NC+N)$。
比較が $O(1)$ なら $O(N)$。

## 空間計算量

Cartesian Tree全体の一時結果と戻り値を含めて $O(N)$。

## 注意点

- `Compare` はstrict weak orderingを満たさなければならない。
- 戻り値と入力のindexは0-indexed。
