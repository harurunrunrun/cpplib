---
title: Wavelet Tree (ウェーブレット木)
documentation_of: ../../../../src/structure/wavelet_matrix/static/wavelet_tree.hpp
---

静的な列の値域（座標圧縮後）を再帰的に二分割した木を作り、区間内の値の頻度と順序統計量を求める。

## API

```cpp
WaveletTree<T>()
explicit WaveletTree<T>(const vector<T>& sequence)
int size() const noexcept
int count_less(int left, int right, const T& value) const
int count(int left, int right, const T& lower, const T& upper) const
T kth_smallest(int left, int right, int k) const
```

- コンストラクタ: `sequence` から構築する。引数なしでは空の列になる。
- `size`: 列の長さを返す。
- `count_less`: 区間 $[\mathit{left}, \mathit{right})$ にある `value` 未満の値の個数を返す。
- `count`: 区間 $[\mathit{left}, \mathit{right})$ にあり、値が $[\mathit{lower}, \mathit{upper})$ に含まれる要素の個数を返す。
- `kth_smallest`: 区間 $[\mathit{left}, \mathit{right})$ で `k` 番目（0-indexed）に小さい値を返す。

`T` は `<` で比較でき、`==` を持つ型であればよい。

## 時間計算量

$N$ を列長、$\sigma$ を相異なる値の個数とする。

- 構築: $O(N \log N + N \log \sigma)$
- `count_less`, `count`, `kth_smallest`: $O(\log \sigma)$
- `size`: $O(1)$

## 空間計算量

各ノードが要素数ぶんの `int` の累積個数を持つため、$O(N \log \sigma)$ ワード。

## 注意点

- 位置の区間・値の区間はいずれも半開区間である。
- 位置の区間が不正な場合や、`k` が $[0, \mathit{right} - \mathit{left})$ の範囲外の場合は `std::out_of_range` を送出する。
- `count` で `lower > upper` を渡しても例外にはならず、負の値を返す。
