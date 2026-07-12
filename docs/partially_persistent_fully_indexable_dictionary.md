---
title: Partially Persistent Fully Indexable Dictionary
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_fully_indexable_dictionary.hpp
---

部分永続の完備辞書。更新は最新バージョンに対してのみ行う。
過去のバージョンは `rank` / `select` / `get` で参照できる。

# 更新

```cpp
int nv = fid.set(k, value)
int nv = fid.flip(k)
```

# 時間計算量

`PersistentFullyIndexableDictionary` と同じ。
