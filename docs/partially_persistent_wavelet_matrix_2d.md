---
title: Partially Persistent Wavelet Matrix 2D (部分永続二次元ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d.hpp
---

x座標を固定し、y座標を部分永続に更新できる2次元wavelet matrix。更新は最新版に対してのみ行う。

```cpp
wm.set_y(k, y)
```

## 時間計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$V$ を現在のversion数、x範囲に入る点数を $L$、触れるblock数を $C$ とし、
$Q_0(L)=B+C\log B$、$Q_V(L)=B+C(\log B+\log(V+1))$ とおく。

- 3種類のconstructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}B+N(\log N+\log B))$
- `size`, `versions`, `latest_version`, `x`: $O(1)$
- `y`: 最新versionは $O(1)$、過去versionは $O(\log(V+1))$
- `set_y`: 償却 $O(B\log B)$。履歴vector再確保時の1回の最悪は $O(B\log B+V)$
- `rectangle_count`, `range_freq`: 最新versionは $O(\log N+Q_0(L))$、過去versionは $O(\log N+Q_V(L))$
- `kth_smallest_y`, `prev_y`, `next_y`: 最新versionは $O(\log N+\mathtt{Y\_BIT\_WIDTH}\,Q_0(L))$、過去versionは $O(\log N+\mathtt{Y\_BIT\_WIDTH}\,Q_V(L))$

## 注意点

点idは入力順、x座標は固定。`x(k)` はx、`y(version,k)` は指定versionのy。
`set_y(k,y)` は最新版から新versionを作り、queryは任意の有効versionを読む。
矩形は `[xl,xr) x [yl,yr)`、順序統計は0-indexed、前後値は該当なしなら `nullopt`。

不正なversion・点・矩形・順序、vector長不一致、容量、yのbit幅では `runtime_error`。
copyは禁止、moveは可能。各APIの計算量は上記表の通り。

## Constructor signature

```cpp
PartiallyPersistentWaveletMatrix2D()
PartiallyPersistentWaveletMatrix2D(const vector<X>& xs, const vector<Y>& ys)
PartiallyPersistentWaveletMatrix2D(const vector<pair<X,Y>>& points)
```
