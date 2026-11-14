---
title: Dynamic Wavelet Matrix 2D (動的二次元ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_wavelet_matrix_2d.hpp
---

点の挿入・削除・座標の更新と、半開矩形内の点数や順序統計を扱う動的な二次元データ構造。点の入力順を implicit treap で、$x$ 座標順を順序統計付きの treap で、$x$ 座標順に並べた $y$ 座標の列を動的ウェーブレット行列で管理する。平方分割や更新ごとの全体の再構築は行わない。

内部では、重みをすべて 0 とした `DynamicWaveletMatrix2DWeighted<X, Y, int, MAX_SIZE, Y_BIT_WIDTH>` を使う。

## テンプレート引数

```cpp
DynamicWaveletMatrix2D<X, Y, MAX_SIZE = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

- `X`, `Y`: 座標の整数型
- `MAX_SIZE`: 同時に保持できる点数の上限。省略時（`-1`）は上限なしで、内部の配列を必要に応じて伸長する。非負の値を指定すると、内部の treap が $O(\texttt{MAX_SIZE})$ の固定長配列を確保する。
- `Y_BIT_WIDTH`: $y$ 座標に使う bit 数。省略時は `Y` の bit 数。符号付きの `Y` では省略しなければならない（`static_assert`）。

```cpp
using Variable = DynamicWaveletMatrix2D<X, Y>;
using Fixed = DynamicWaveletMatrix2D<X, Y, N>;
```

## 構築と要素参照

$D = \texttt{Y_BIT_WIDTH}$ とする。

| API | 説明 | 時間計算量 |
|---|---|---|
| `DynamicWaveletMatrix2D()` | 空集合を構築する。 | $O(D)$ |
| `DynamicWaveletMatrix2D(xs, ys)` | 同じ長さの座標列から構築する。 | 期待 $O(N \log N + N D)$ |
| `explicit DynamicWaveletMatrix2D(points)` | `pair<X, Y>` の列から構築する。 | 期待 $O(N \log N + N D)$ |
| `size()`, `empty()` | 点数を返す / 空かどうかを返す。 | $O(1)$ |
| `x(k)`, `y(k)` | 現在の入力順で `k` 番目の点の座標を返す。 | 期待 $O(\log N)$ |

## 挿入・削除・更新

| API | 説明 | 時間計算量 |
|---|---|---|
| `insert(k, x, y)` | 入力順の位置 `k` に点を挿入する。 | 期待 $O(D \log N)$ |
| `push_back(x, y)` | 末尾に点を挿入する。 | 期待 $O(D \log N)$ |
| `erase(k)` | `k` 番目の点を削除し、`pair<X, Y>` で返す。 | 期待 $O(D \log N)$ |
| `pop_back()` | 末尾の点を削除して返す。 | 期待 $O(D \log N)$ |
| `set_y(k, y)` | `k` 番目の点の $y$ 座標を変更する。 | 期待 $O(D \log N)$ |
| `set_x(k, x)` | `k` 番目の点の $x$ 座標を変更する。 | 期待 $O(D \log N)$ |
| `set(k, x, y)` | `k` 番目の点の両方の座標を変更する。 | 期待 $O(D \log N)$ |

挿入・削除の後の添字は、通常の配列と同様にずれる。同じ座標の点を重複して保持できる。

## 矩形・順序統計クエリ

| API | 説明 | 時間計算量 |
|---|---|---|
| `rectangle_count(xl, xr, yl, yr)` | $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の個数。 | 期待 $O(D \log N)$ |
| `range_freq(xl, xr, yl, yr)` | `rectangle_count` の別名。 | 期待 $O(D \log N)$ |
| `kth_smallest_y(xl, xr, k)` | $x$ 座標が $[\mathit{xl}, \mathit{xr})$ に入る点の $y$ 座標のうち、`k` 番目（0-indexed）に小さい値。 | 期待 $O(D \log N)$ |
| `prev_y(xl, xr, upper)` | $x$ 範囲内で `upper` 未満の最大の $y$ 座標。なければ `nullopt`。 | 期待 $O(D \log N)$ |
| `next_y(xl, xr, lower)` | $x$ 範囲内で `lower` 以上の最小の $y$ 座標。なければ `nullopt`。 | 期待 $O(D \log N)$ |

## 時間計算量

treap の高さに依存する部分は期待計算量である。各更新・クエリは $y$ の各 bit 階層で動的ビット列をたどるため、期待 $O(D \log N)$ となる（表を参照）。

## 空間計算量

$O(N D)$（固定長版では treap の配列に $O(\texttt{MAX_SIZE})$ が加わる）。

## 注意点

- 添字は 0-indexed、座標の範囲は半開区間である。
- `insert` の後も点数が `MAX_SIZE` 以下でなければならない。
- 無効な添字、容量超過、`Y_BIT_WIDTH` bit に収まらない $y$ 座標、`xl > xr` または `yl > yr` の矩形、範囲外の `k` では `std::runtime_error` を送出する。
- コピーは禁止、ムーブは可能。

## 使用例

```cpp
DynamicWaveletMatrix2D<int, int, 100000> wm;
wm.push_back(3, 7);
wm.insert(0, 1, 4);
wm.set_x(1, 2);
int count = wm.rectangle_count(0, 3, 0, 10);
wm.erase(0);
```
