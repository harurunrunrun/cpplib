---
title: Wavelet Tree (ウェーブレット木)
documentation_of: ../../../../src/structure/wavelet_matrix/static/wavelet_tree.hpp
---

静的列の値域を再帰分割し、区間頻度と順序統計量を処理する。

## API

- `WaveletTree(sequence)`: 列を構築する。
- `size()`: 長さを返す。
- `count_less(left,right,value)`: 値未満の個数を返す。
- `count(left,right,lower,upper)`: 値区間内の個数を返す。
- `kth_smallest(left,right,k)`: 0-originで `k` 番目を返す。

## 時間計算量

- 構築: $O(N\log \sigma)$。
- 各クエリ: $O(\log \sigma)$。
- `size`: $O(1)$。

## 注意点

- $\sigma$ は異なる値の個数。値区間・位置区間はいずれも半開区間。
