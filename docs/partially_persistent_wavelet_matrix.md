---
title: Partially Persistent Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix.hpp
---

部分永続の点更新wavelet matrix。
更新は最新バージョンに対してのみ行う。
過去のバージョンはクエリで参照できる。

# 更新

```cpp
int nv = wm.set(k, value)
```

# 時間計算量

`PersistentWaveletMatrix` と同じ。
