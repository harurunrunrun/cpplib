---
title: Persistent Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d.hpp
---

x座標を固定し、y座標を完全永続に更新できる2次元wavelet matrix。

# 更新

```cpp
wm.set_y(version, k, y)
wm.fork(version)
```

元の点番号 `k` のy座標を更新する。x座標は変えない。

# クエリ

```cpp
wm.rectangle_count(version, xl, xr, yl, yr)
wm.range_freq(version, xl, xr, yl, yr)
wm.kth_smallest_y(version, xl, xr, k)
wm.prev_y(version, xl, xr, upper)
wm.next_y(version, xl, xr, lower)
```
# 時間計算量

x 座標の範囲取得に $O(\log N)$、以降は `PersistentWaveletMatrix` と同じ。
`set_y` は $O(B\log B + \log M)$、`fork` は $O(1)$。

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$D=\mathtt{Y\_BIT\_WIDTH}$、$M=\lceil N/B\rceil$、
$M_{max}=\lceil\mathtt{MAX\_SIZE}/B\rceil$ とする。
x範囲に入る $L$ 点が触れるblock数を $C$ とし、
$Q(L)=B+C(\log B+\log(M+1))$ とおく。

- 3種類のconstructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}(B+\log(M_{max}+1))+N(\log N+\log B))$
- `size`, `versions`, `latest_version`, `x`, `fork`: $O(1)$
- `y`: $O(\log(M+1))$
- `set_y`: $O(B\log B+\log(M+1))$
- `rectangle_count`, `range_freq`: $O(\log N+Q(L))$
- `kth_smallest_y`, `prev_y`, `next_y`: $O(\log N+DQ(L))$

# Version・座標・例外

点idは入力順、x座標は全versionで固定。`x(k)` はx、`y(version,k)` は指定versionのy。
`set_y` は指定versionから新versionを返し、`fork` は同内容で分岐する。
矩形は `[xl,xr) x [yl,yr)`、順序統計は0-indexed、前後値は該当なしなら `nullopt`。

不正なversion・点・矩形・順序、vector長不一致、点数/version/block容量、yのbit幅では
`runtime_error`。失敗した更新はversionと使用量を戻す。copyは禁止、moveは可能。
各APIの計算量は上記表の通り。

# Constructor signature

```cpp
PersistentWaveletMatrix2D()
PersistentWaveletMatrix2D(const vector<X>& xs, const vector<Y>& ys)
PersistentWaveletMatrix2D(const vector<pair<X,Y>>& points)
```
