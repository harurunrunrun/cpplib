---
title: Static Range K-th Smallest (静的区間K番目最小値) [range_kth_smallest]
documentation_of: ../../../../src/structure/range_query/value_query/static_range_kth_smallest.hpp
---

静的な配列の半開区間に含まれる値のうち、$k$ 番目に小さい値を返す。値を座標圧縮し、各接頭辞の頻度を永続セグメント木で保持する。

## テンプレート引数

```cpp
template<class Value, int MAX_SIZE = -1>
class StaticRangeKthSmallest;
```

`Value` は要素の型、`MAX_SIZE` は配列の長さの上限である。`MAX_SIZE` を省略した場合（`-1`）は上限を設けず、実行時の長さに合わせて `std::vector` で領域を確保する。コピーはできず、ムーブはできる。

## コンストラクタ

```cpp
explicit StaticRangeKthSmallest(const vector<Value>& values)
```

`values` から索引を構築する。

時間計算量は、配列の長さを $N$ として $O(\texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}+1)+N\log(N+1))$（容量省略時は $O(N\log(N+1))$）である。

## `kth_smallest`

```cpp
Value kth_smallest(int left, int right, int order) const
```

半開区間 `[left, right)` の値を昇順に並べたときの、0-indexed で `order` 番目の値を返す。重複する値はそれぞれ数える。

時間計算量は $O(\log(\texttt{MAX_SIZE}+1))$（容量省略時は $O(\log(N+1))$）である。

## `size` / `empty`

```cpp
int size() const
bool empty() const
```

配列の長さと、配列が空かどうかを返す。時間計算量はどちらも $O(1)$。

## 空間計算量

$O(\texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}+1))$（容量省略時は $O(N\log(N+1))$）。

## 注意点

- `Value` はコピー可能で、`operator<` が狭義弱順序を満たす必要がある。
- `values.size() > MAX_SIZE` なら `std::length_error` を送出する。
- クエリ区間は空でない有効な半開区間で、`order` は $[0, \mathit{right}-\mathit{left})$ に含まれなければならない。違反時は `std::out_of_range` を送出する。
