---
title: Partially Persistent Weighted Wavelet Matrix 2D (部分永続二次元重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_wavelet_matrix_2d_weighted.hpp
---

入力順の点idとx座標を固定し、最新版のy座標と重みを更新して過去versionを参照できる二次元Wavelet Matrix。
完全永続型をラップしないlatest-onlyの重み付きB+木実装を使用する。

## API

```cpp
PartiallyPersistentWaveletMatrix2DWeighted<
    X,Y,W,MAX_SIZE,MAX_VERSION,Y_BIT_WIDTH
> wm(xs, ys, weights)
X x = wm.x(k)
Y y = wm.y(version, k)
W weight = wm.weight(version, k)
int version2 = wm.set(k, y, weight)
int version2 = wm.set_y(k, y)
int version2 = wm.set_weight(k, weight)
int count = wm.rectangle_count(version, xl, xr, yl, yr)
W total = wm.rectangle_sum(version, xl, xr, yl, yr)
```

`range_freq/range_sum` は矩形queryの別名。矩形は半開で、queryは任意の有効versionを読む。

## 時間計算量

$D=\mathtt{Y\_BIT\_WIDTH}$、$H=O(\log(N+1))$。

- 構築: $O(\mathtt{MAX\_SIZE}+N\log N+DN)$
- `size/versions/latest_version/x`: $O(1)$
- `weight`: $O(H)$、`y`: $O(DH)$
- `set/set_y/set_weight`: $O(DH)$
- 矩形count/sumと別名: $O(\log N+DH)$

最新版と過去versionのquery計算量は同じ。更新1回の追加メモリは $O(DH)$。

## 注意点

更新元は最新版だけ。同じxの点も入力idで区別する。copyは禁止、moveは可能。
不正なversion・点・矩形、入力長不一致、yのbit幅、容量では `runtime_error`。
失敗した更新はversion数と全bit段のnode使用量を戻す。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE`, `MAX_VERSION` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = PartiallyPersistentWaveletMatrix2DWeighted<X, Y, W>;
using Fixed = PartiallyPersistentWaveletMatrix2DWeighted<X, Y, W, N, Versions>;
```

可変長版は必要なノード・版・履歴を明示的に追加し、保存要素数を $K$ として追加領域は $O(K)$。
再確保の費用は追加操作全体で償却され、上記各操作の計算量を維持する。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
