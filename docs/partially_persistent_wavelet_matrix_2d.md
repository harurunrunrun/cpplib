---
title: Partially Persistent Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d.hpp
---

x座標を固定し、y座標を部分永続に更新できる2次元wavelet matrix。更新は最新版に対してのみ行う。

```cpp
wm.set_y(k, y)
```
