---
title: Persistent Weighted Wavelet Matrix 2D (完全永続二次元重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_wavelet_matrix_2d_weighted.hpp
---

入力順の点idとx座標を固定し、y座標と重みを任意versionから更新できる完全永続二次元Wavelet Matrix。
x順に並べた点列を永続重み付きWavelet Matrixで管理する。

## API

```cpp
PersistentWaveletMatrix2DWeighted<X,Y,W,MAX_SIZE,MAX_VERSION,Y_BIT_WIDTH> wm(xs, ys, weights)
X x = wm.x(k)
Y y = wm.y(version, k)
W weight = wm.weight(version, k)
int version2 = wm.set(version, k, y, weight)
int version2 = wm.set_y(version, k, y)
int version2 = wm.set_weight(version, k, weight)
int version2 = wm.fork(version)
int count = wm.rectangle_count(version, xl, xr, yl, yr)
W total = wm.rectangle_sum(version, xl, xr, yl, yr)
```

`range_freq` は `rectangle_count`、`range_sum` は `rectangle_sum` と同じ。
コンストラクタは3本のvectorまたは `tuple<X,Y,W>` のvectorも受け取る。
矩形は $[x_l,x_r)\times[y_l,y_r)$。

## 時間計算量

$D=\mathtt{Y\_BIT\_WIDTH}$、$H=O(\log(N+1))$。

- 構築: $O(\mathtt{MAX\_SIZE}+N\log N+DN)$
- `size/versions/latest_version/x`: $O(1)$
- `weight`: $O(H)$、`y`: $O(DH)$、`fork`: $O(D)$
- `set/set_y/set_weight`: $O(DH)$
- 矩形count/sumと別名: $O(\log N+DH)$

更新1回の追加メモリは $O(DH)$。平方分割は使用しない。

## 注意点

同じxの点も入力idで区別する。copyは禁止、moveは可能。
不正なversion・点・矩形、入力長不一致、yのbit幅、容量では `runtime_error`。
失敗した更新はversion数と全bit段のnode使用量を戻す。
