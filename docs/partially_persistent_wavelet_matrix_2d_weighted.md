---
title: Partially Persistent Weighted Wavelet Matrix 2D (部分永続二次元重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d_weighted.hpp
---

x座標を固定し、y座標と重みを部分永続に更新できる2次元wavelet matrix。

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$V$ を現在のversion数、x範囲に入る点数を $L$、触れるblock数を $C$ とし、
$Q_0(L)=B+C\log B$、$Q_V(L)=B+C(\log B+\log(V+1))$ とおく。

- 3種類のconstructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}B+N(\log N+\log B))$
- `size`, `versions`, `latest_version`, `x`: $O(1)$
- `y`, `weight`: 最新versionは $O(1)$、過去versionは $O(\log(V+1))$
- `set`, `set_y`, `set_weight`: 償却 $O(B\log B)$。履歴vector再確保時の1回の最悪は $O(B\log B+V)$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: 最新versionは $O(\log N+Q_0(L))$、過去versionは $O(\log N+Q_V(L))$

# Version・座標・例外

点idは入力順、xは固定。`x(k)`、`y(version,k)`、`weight(version,k)` は1点を返す。
`set/set_y/set_weight` は最新版から新versionを作り、矩形queryは任意versionを読む。
count/freqは半開矩形の点数、sumは重み総和。

不正なversion・点・矩形、vector長不一致、各容量、yのbit幅では `runtime_error`。
copyは禁止、moveは可能。各APIの計算量は上記表の通り。

# Constructor signature

```cpp
PartiallyPersistentWaveletMatrix2DWeighted()
PartiallyPersistentWaveletMatrix2DWeighted(const vector<X>& xs, const vector<Y>& ys, const vector<W>& weights)
PartiallyPersistentWaveletMatrix2DWeighted(const vector<tuple<X,Y,W>>& points)
```
