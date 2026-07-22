---
title: Dynamic Weighted Wavelet Matrix 2D (動的二次元重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d_weighted.hpp
---

点の挿入・削除、座標・重み更新、半開矩形内の点数と重み和を扱う動的二次元データ構造。
入力順のimplicit treap、x座標順の順序統計付きtreap、重み付き動的Wavelet Matrixを組み合わせる。
平方分割や更新ごとの全体再構築は行わない。

## テンプレート引数

```cpp
DynamicWaveletMatrix2DWeighted<X, Y, W, MAX_SIZE, Y_BIT_WIDTH>
```

`X` と `Y` は整数座標型、`W` は加減算できる重み型、`MAX_SIZE` は点数上限。

## 構築と要素参照

| API | 説明 | 時間計算量 |
|---|---|---|
| default constructor | 空集合を構築する。 | $O(Y\_BIT\_WIDTH)$ |
| `DynamicWaveletMatrix2DWeighted(xs,ys,weights)` | 3本の同じ長さの列から構築する。 | 期待 $O(N\log N+N\,Y\_BIT\_WIDTH)$ |
| `DynamicWaveletMatrix2DWeighted(points)` | `tuple<X,Y,W>` の列から構築する。 | 期待 $O(N\log N+N\,Y\_BIT\_WIDTH)$ |
| `size()`, `empty()` | 点数、空判定。 | $O(1)$ |
| `x(k)`, `y(k)`, `weight(k)` | 入力順で第 `k` 点の現在値。 | 期待 $O(\log N)$ |

## 挿入・削除・更新

| API | 説明 | 時間計算量 |
|---|---|---|
| `insert(k,x,y,w)` | 入力順の位置 `k` に点を挿入する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `push_back(x,y,w)` | 末尾へ点を挿入する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `erase(k)` | 第 `k` 点を削除し `tuple<X,Y,W>` で返す。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `pop_back()` | 末尾の点を削除して返す。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `set(k,y,w)` | y座標と重みを更新する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `set_y(k,y)`, `set_weight(k,w)` | y座標または重みだけを更新する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `set_x(k,x)` | x座標を更新する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `set_point(k,x,y,w)` | 座標と重みをまとめて更新する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |

挿入・削除後の添字は配列と同様にずれる。同じ座標の点を複数保持できる。

## 矩形・順序統計クエリ

| API | 説明 | 時間計算量 |
|---|---|---|
| `rectangle_count(xl,xr,yl,yr)`, `range_freq(...)` | $[xl,xr)\times[yl,yr)$ の点数。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `rectangle_sum(xl,xr,yl,yr)`, `range_sum(...)` | 同じ矩形内の重み総和。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `kth_smallest_y(xl,xr,k)` | x範囲内で0-indexed第 `k` 小さいy座標。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `prev_y(xl,xr,upper)` | `upper` 未満の最大y。なければ `nullopt`。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `next_y(xl,xr,lower)` | `lower` 以上の最小y。なければ `nullopt`。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |

## 時間計算量

treapの高さに対する計算量は期待計算量である。更新・クエリはyの各bit階層にある
動的bit列と部分和を辿るため、基本的に期待 $O(Y\_BIT\_WIDTH\log N)$。
空間計算量は $O(N\,Y\_BIT\_WIDTH)$。

## 注意点

添字は0-indexed、矩形は半開区間。`MAX_SIZE` を超える挿入はできない。
符号付き `Y` では `Y_BIT_WIDTH` に型の全bit幅を指定する。
無効な添字・矩形、容量超過、bit幅外の値では `runtime_error`。
copyは禁止、moveは可能。

## 使用例

```cpp
DynamicWaveletMatrix2DWeighted<int, int, long long, 100000> wm;
wm.push_back(1, 4, 10);
wm.insert(0, 3, 2, -5);
wm.set_point(1, 2, 7, 20);
long long sum = wm.rectangle_sum(0, 4, 0, 8);
wm.erase(0);
```
