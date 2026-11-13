---
title: Dynamic Rectangle Sum (動的長方形和)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_rectangle_sum.hpp
---

`DynamicWaveletMatrix2DWeighted` を矩形和の用途の名前で公開する派生型。点集合を動的に変更しながら、半開矩形内の点の個数と重みの総和を求める。平方分割や更新ごとの全体の再構築は行わない。

## テンプレート引数

```cpp
DynamicRectangleSum<X, Y, W, MAX_SIZE = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

意味は `DynamicWaveletMatrix2DWeighted` と同じである。

```cpp
using Variable = DynamicRectangleSum<X, Y, W>;
using Fixed = DynamicRectangleSum<X, Y, W, N>;
```

## API

コンストラクタを含め、基底型の全公開 API を利用できる。

| 分類 | API |
|---|---|
| 構築・参照 | コンストラクタ, `size`, `empty`, `x`, `y`, `weight` |
| 挿入・削除 | `insert`, `push_back`, `erase`, `pop_back` |
| 更新 | `set`, `set_x`, `set_y`, `set_weight`, `set_point` |
| 矩形クエリ | `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum` |
| $y$ の順序統計 | `kth_smallest_y`, `prev_y`, `next_y` |

## 時間計算量

$D = \texttt{Y_BIT_WIDTH}$、現在の点数を $N$ とする。

- `size`, `empty`: $O(1)$
- `x`, `y`, `weight`: 期待 $O(\log N)$
- 挿入・削除・更新: 期待 $O(D \log N)$
- 矩形・順序統計クエリ: 期待 $O(D \log N)$
- 構築: 期待 $O(N \log N + N D)$

## 空間計算量

$O(N D)$（固定長版では treap の配列に $O(\texttt{MAX_SIZE})$ が加わる）。

## 注意点

- 添字は入力順の 0-indexed である。挿入・削除で後続の添字はずれる。
- 矩形は $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ である。
- `MAX_SIZE` を超える挿入、無効な添字・矩形、`Y_BIT_WIDTH` bit に収まらない $y$ 座標では `std::runtime_error` を送出する。
- 同じ座標の点を複数保持できる。

## 使用例

```cpp
DynamicRectangleSum<int, int, long long, 100000> points;
points.push_back(1, 2, 5);
points.push_back(3, 4, 7);
auto sum = points.rectangle_sum(0, 4, 0, 5);
points.set_weight(0, 9);
points.erase(1);
```
