---
title: Static Range K-th Smallest (静的区間K番目最小値) [range_kth_smallest]
documentation_of: ../src/structure/other/static_range_kth_smallest.hpp
---

静的配列の半開区間に含まれる値のk番目最小値を返す。値は座標圧縮し、各prefixの
頻度を永続セグメント木で保持する。

## テンプレート引数

```cpp
StaticRangeKthSmallest<Value, MAX_SIZE>
```

`Value` は要素型、`MAX_SIZE` は配列長の上限。copyはできず、moveはできる。

## コンストラクタ

```cpp
explicit StaticRangeKthSmallest(const vector<Value>& values)
```

`values` から索引を構築する。

## 時間計算量

配列長を $N$ として
$O(MAX\_SIZE\log(MAX\_SIZE+1)+N\log(N+1))$。

## `kth_smallest`

```cpp
Value kth_smallest(int left, int right, int order) const
```

半開区間 $[left,right)$ の値を昇順に並べたときの0-indexed `order` 番目を返す。
重複する値はそれぞれ数える。

## 時間計算量

$O(\log(MAX\_SIZE+1))$。

## `size` / `empty`

```cpp
int size() const
bool empty() const
```

配列長、配列が空かどうかを返す。

## 時間計算量

どちらも $O(1)$。

## 空間計算量

$O(MAX\_SIZE\log(MAX\_SIZE+1))$。

## 注意点

`Value` はcopy可能で、`operator<` がstrict weak orderingを満たす必要がある。
`values.size() > MAX_SIZE` では `length_error` を送出する。query区間は空でない
有効な半開区間で、`order` は $[0,right-left)$ に含まれなければならず、違反時は
`out_of_range` を送出する。
