---
title: Dynamic Wavelet Matrix 2D (動的二次元ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d.hpp
---

点の挿入・削除、座標更新、および半開矩形内の順序統計を扱う動的二次元データ構造。
入力順は implicit treap、x座標順は順序統計付きtreap、y座標列は動的Wavelet Matrixで管理する。
平方分割や更新ごとの全体再構築は行わない。

## テンプレート引数

```cpp
DynamicWaveletMatrix2D<X, Y, MAX_SIZE, Y_BIT_WIDTH, BLOCK_SIZE>
```

- `X`, `Y`: 整数座標型。
- `MAX_SIZE`: 同時に保持できる点数の上限。
- `Y_BIT_WIDTH`: y座標を表現するbit数。
- `BLOCK_SIZE`: 後方互換用で、内部の動的木の計算量には影響しない。

## 構築と要素参照

| API | 説明 | 時間計算量 |
|---|---|---|
| `DynamicWaveletMatrix2D()` | 空集合を構築する。 | $O(Y\_BIT\_WIDTH)$ |
| `DynamicWaveletMatrix2D(xs, ys)` | 同じ長さの座標列から構築する。 | 期待 $O(N\log N+N\,Y\_BIT\_WIDTH)$ |
| `DynamicWaveletMatrix2D(points)` | `pair<X,Y>` の列から構築する。 | 期待 $O(N\log N+N\,Y\_BIT\_WIDTH)$ |
| `size()`, `empty()` | 点数、空判定を返す。 | $O(1)$ |
| `x(k)`, `y(k)` | 現在の入力順で第 `k` 点の座標を返す。 | 期待 $O(\log N)$ |

## 挿入・削除・更新

| API | 説明 | 時間計算量 |
|---|---|---|
| `insert(k, x, y)` | 入力順の位置 `k` に点を挿入する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `push_back(x, y)` | 末尾へ点を挿入する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `erase(k)` | 第 `k` 点を削除し、`pair<X,Y>` で返す。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `pop_back()` | 末尾の点を削除して返す。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `set_y(k, y)` | 第 `k` 点のy座標を変更する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `set_x(k, x)` | 第 `k` 点のx座標を変更する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `set(k, x, y)` | 第 `k` 点の両座標を変更する。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |

挿入・削除後の添字は通常の配列と同様にずれる。点は同じ座標を重複して保持できる。

## 矩形・順序統計クエリ

| API | 説明 | 時間計算量 |
|---|---|---|
| `rectangle_count(xl,xr,yl,yr)` | $[xl,xr)\times[yl,yr)$ の点数。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `range_freq(xl,xr,yl,yr)` | `rectangle_count` の別名。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `kth_smallest_y(xl,xr,k)` | x範囲内のy座標のうち0-indexedで第 `k` 小さい値。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `prev_y(xl,xr,upper)` | x範囲内で `upper` 未満の最大y。なければ `nullopt`。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |
| `next_y(xl,xr,lower)` | x範囲内で `lower` 以上の最小y。なければ `nullopt`。 | 期待 $O(Y\_BIT\_WIDTH\log N)$ |

## 時間計算量

treapの高さに対する計算量は期待計算量である。各更新・クエリは平方分割に依存せず、
yの各bit階層で動的bit列を辿るため、基本的に期待 $O(Y\_BIT\_WIDTH\log N)$。

## 注意点

添字は0-indexed、座標範囲は半開区間。`insert` 後も点数が `MAX_SIZE` 以下でなければならない。
符号付き `Y` では `Y_BIT_WIDTH` に型の全bit幅を指定する。
無効な添字、容量超過、bit幅外の値、逆転した矩形では `runtime_error`。
copyは禁止、moveは可能。

## 使用例

```cpp
DynamicWaveletMatrix2D<int, int, 100000> wm;
wm.push_back(3, 7);
wm.insert(0, 1, 4);
wm.set_x(1, 2);
int count = wm.rectangle_count(0, 3, 0, 10);
wm.erase(0);
```
