---
title: Persistent Weighted Wavelet Matrix 2D (完全永続二次元重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d_weighted.hpp
---

x座標を固定し、y座標と重みを完全永続に更新できる2次元wavelet matrix。

```cpp
wm.set(version, k, y, weight)
wm.set_y(version, k, y)
wm.set_weight(version, k, weight)
wm.rectangle_count(version, xl, xr, yl, yr)
wm.rectangle_sum(version, xl, xr, yl, yr)
```

## 時間計算量

x 座標の範囲取得に $O(\log N)$、以降は
`PersistentWeightedWaveletMatrix` と同じ。
`set`, `set_y`, `set_weight` は $O(B\log B + \log M)$、
`fork` は $O(1)$。

## 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$M=\lceil N/B\rceil$、
$M_{max}=\lceil\mathtt{MAX\_SIZE}/B\rceil$ とする。
x範囲に入る $L$ 点が触れるblock数を $C$ とし、
$Q(L)=B+C(\log B+\log(M+1))$ とおく。

- 3種類のconstructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}(B+\log(M_{max}+1))+N(\log N+\log B))$
- `size`, `versions`, `latest_version`, `x`, `fork`: $O(1)$
- `y`, `weight`: $O(\log(M+1))$
- `set`, `set_y`, `set_weight`: $O(B\log B+\log(M+1))$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N+Q(L))$

## 注意点

点idは入力順、xは全versionで固定。`x(k)`、`y(version,k)`、`weight(version,k)` は1点を返す。
`set` は指定versionのyと重み、`set_y/set_weight` は片方を更新した新versionを返し、
`fork` は同内容で分岐する。count/freqは半開矩形の点数、sumは重み総和。

不正なversion・点・矩形、vector長不一致、点数/version/block容量、yのbit幅では
`runtime_error`。失敗時にversionと使用量は増えない。copyは禁止、moveは可能。
各APIの計算量は上記表の通り。

## Constructor signature

```cpp
PersistentWaveletMatrix2DWeighted()
PersistentWaveletMatrix2DWeighted(const vector<X>& xs, const vector<Y>& ys, const vector<W>& weights)
PersistentWaveletMatrix2DWeighted(const vector<tuple<X,Y,W>>& points)
```
