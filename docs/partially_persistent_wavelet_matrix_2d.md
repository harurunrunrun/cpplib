---
title: Partially Persistent Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d.hpp
---

x座標を固定し、y座標を部分永続に更新できる2次元wavelet matrix。更新は最新版に対してのみ行う。

```cpp
wm.set_y(k, y)
```

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$V$ を現在のversion数、x範囲に入る点数を $L$、触れるblock数を $C$ とし、
$Q_0(L)=B+C\log B$、$Q_V(L)=B+C(\log B+\log(V+1))$ とおく。

- 3種類のconstructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}B+N(\log N+\log B))$
- `size`, `versions`, `latest_version`, `x`: $O(1)$
- `y`: 最新versionは $O(1)$、過去versionは $O(\log(V+1))$
- `set_y`: 償却 $O(B\log B)$。履歴vector再確保時の1回の最悪は $O(B\log B+V)$
- `rectangle_count`, `range_freq`: 最新versionは $O(\log N+Q_0(L))$、過去versionは $O(\log N+Q_V(L))$
- `kth_smallest_y`, `prev_y`, `next_y`: 最新versionは $O(\log N+\mathtt{Y\_BIT\_WIDTH}\,Q_0(L))$、過去versionは $O(\log N+\mathtt{Y\_BIT\_WIDTH}\,Q_V(L))$
