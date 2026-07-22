---
title: Dynamic Rectangle Sum (動的長方形和)
documentation_of: ../src/structure/wavelet_matrix/dynamic_rectangle_sum.hpp
---

`DynamicWaveletMatrix2DWeighted` を矩形和用途の名前で公開する派生型。
点集合を動的に変更しながら、半開矩形内の点数と重み総和を求める。
平方分割や更新ごとの全体再構築は行わない。

## API

基底型の全公開APIを利用できる。

| 分類 | API |
|---|---|
| 構築・参照 | constructors, `size`, `empty`, `x`, `y`, `weight` |
| 挿入・削除 | `insert`, `push_back`, `erase`, `pop_back` |
| 更新 | `set`, `set_x`, `set_y`, `set_weight`, `set_point` |
| 矩形クエリ | `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum` |
| y順序統計 | `kth_smallest_y`, `prev_y`, `next_y` |

## 時間計算量

$D=Y\_BIT\_WIDTH$、現在の点数を $N$ とする。

- `size`, `empty`: $O(1)$
- `x`, `y`, `weight`: 期待 $O(\log N)$
- 挿入・削除・更新: 期待 $O(D\log N)$
- 矩形・順序統計クエリ: 期待 $O(D\log N)$
- 構築: 期待 $O(N\log N+ND)$
- 空間: $O(ND)$

## 注意点

添字は入力順の0-indexed。挿入・削除で後続の添字はずれる。
矩形は $[xl,xr)\times[yl,yr)$。`MAX_SIZE` を超える挿入、無効な添字・矩形、
y座標のbit幅違反では `runtime_error`。同じ座標の点を複数保持できる。

## 使用例

```cpp
DynamicRectangleSum<int, int, long long, 100000> points;
points.push_back(1, 2, 5);
points.push_back(3, 4, 7);
auto sum = points.rectangle_sum(0, 4, 0, 5);
points.set_weight(0, 9);
points.erase(1);
```
