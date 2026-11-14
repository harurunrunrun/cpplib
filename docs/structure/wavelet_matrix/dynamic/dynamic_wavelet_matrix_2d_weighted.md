---
title: Dynamic Weighted Wavelet Matrix 2D (動的二次元重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_wavelet_matrix_2d_weighted.hpp
---

点の挿入・削除・座標や重みの更新と、半開矩形内の点数・重みの総和を扱う動的な二次元データ構造。点の入力順を implicit treap で、$x$ 座標順を順序統計付きの treap で管理し、$x$ 座標順に並べた $y$ 座標と重みを重み付き動的ウェーブレット行列で管理する。平方分割や更新ごとの全体の再構築は行わない。

## テンプレート引数

```cpp
DynamicWaveletMatrix2DWeighted<X, Y, W, MAX_SIZE = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

- `X`, `Y`: 座標の整数型
- `W`: 重みの型。デフォルト構築（零元）、加算、減算が必要
- `MAX_SIZE`: 同時に保持できる点数の上限。省略時（`-1`）は上限なしで、内部の配列を必要に応じて伸長する。非負の値を指定すると、内部の treap が $O(\texttt{MAX_SIZE})$ の固定長配列を確保する。
- `Y_BIT_WIDTH`: $y$ 座標に使う bit 数。省略時は `Y` の bit 数。符号付きの `Y` では省略しなければならない（`static_assert`）。

```cpp
using Variable = DynamicWaveletMatrix2DWeighted<X, Y, W>;
using Fixed = DynamicWaveletMatrix2DWeighted<X, Y, W, N>;
```

## 構築と要素参照

$D = \texttt{Y_BIT_WIDTH}$ とする。

| API | 説明 | 時間計算量 |
|---|---|---|
| デフォルトコンストラクタ | 空集合を構築する。 | $O(D)$ |
| `DynamicWaveletMatrix2DWeighted(xs, ys, weights)` | 同じ長さの 3 本の列から構築する。 | 期待 $O(N \log N + N D)$ |
| `explicit DynamicWaveletMatrix2DWeighted(points)` | `tuple<X, Y, W>` の列から構築する。 | 期待 $O(N \log N + N D)$ |
| `size()`, `empty()` | 点数を返す / 空かどうかを返す。 | $O(1)$ |
| `x(k)`, `y(k)`, `weight(k)` | 入力順で `k` 番目の点の現在の値を返す。 | 期待 $O(\log N)$ |

## 挿入・削除・更新

| API | 説明 | 時間計算量 |
|---|---|---|
| `insert(k, x, y, w)` | 入力順の位置 `k` に点を挿入する。 | 期待 $O(D \log N)$ |
| `push_back(x, y, w)` | 末尾に点を挿入する。 | 期待 $O(D \log N)$ |
| `erase(k)` | `k` 番目の点を削除し、`tuple<X, Y, W>` で返す。 | 期待 $O(D \log N)$ |
| `pop_back()` | 末尾の点を削除して返す。 | 期待 $O(D \log N)$ |
| `set(k, y, w)` | $y$ 座標と重みを更新する。 | 期待 $O(D \log N)$ |
| `set_y(k, y)`, `set_weight(k, w)` | $y$ 座標または重みだけを更新する。 | 期待 $O(D \log N)$ |
| `set_x(k, x)` | $x$ 座標を更新する。 | 期待 $O(D \log N)$ |
| `set_point(k, x, y, w)` | 座標と重みをまとめて更新する。 | 期待 $O(D \log N)$ |

挿入・削除の後の添字は、配列と同様にずれる。同じ座標の点を複数保持できる。

## 矩形・順序統計クエリ

| API | 説明 | 時間計算量 |
|---|---|---|
| `rectangle_count(xl, xr, yl, yr)`, `range_freq(...)` | $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の個数。 | 期待 $O(D \log N)$ |
| `rectangle_sum(xl, xr, yl, yr)`, `range_sum(...)` | 同じ矩形内の点の重みの総和。 | 期待 $O(D \log N)$ |
| `kth_smallest_y(xl, xr, k)` | $x$ 範囲内で `k` 番目（0-indexed）に小さい $y$ 座標。 | 期待 $O(D \log N)$ |
| `prev_y(xl, xr, upper)` | $x$ 範囲内で `upper` 未満の最大の $y$ 座標。なければ `nullopt`。 | 期待 $O(D \log N)$ |
| `next_y(xl, xr, lower)` | $x$ 範囲内で `lower` 以上の最小の $y$ 座標。なければ `nullopt`。 | 期待 $O(D \log N)$ |

## 時間計算量

treap の高さに依存する部分は期待計算量である。更新・クエリは $y$ の各 bit 階層にある動的ビット列と部分和をたどるため、期待 $O(D \log N)$ となる（表を参照）。

## 空間計算量

$O(N D)$（固定長版では treap の配列に $O(\texttt{MAX_SIZE})$ が加わる）。

## 注意点

- 添字は 0-indexed、矩形は半開区間である。`MAX_SIZE` を超える挿入はできない。
- 無効な添字・矩形、容量超過、`Y_BIT_WIDTH` bit に収まらない $y$ 座標、範囲外の `k` では `std::runtime_error` を送出する。
- コピーは禁止、ムーブは可能。

## 使用例

```cpp
DynamicWaveletMatrix2DWeighted<int, int, long long, 100000> wm;
wm.push_back(1, 4, 10);
wm.insert(0, 3, 2, -5);
wm.set_point(1, 2, 7, 20);
long long sum = wm.rectangle_sum(0, 4, 0, 8);
wm.erase(0);
```
