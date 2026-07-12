---
title: Persistent Fully Indexable Dictionary
documentation_of: ../src/structure/wavelet_matrix/persistent_fully_indexable_dictionary.hpp
---

完全永続の完備辞書。任意のバージョンから分岐してbitを更新できる。

# 更新

```cpp
int nv = fid.set(version, k, value)
int nv = fid.flip(version, k)
int nv = fid.fork(version)
```

# クエリ

```cpp
fid.get(version, k)
fid.rank(version, value, r)
fid.rank(version, value, l, r)
fid.select(version, value, k)
```

# 時間計算量

- 更新: $O(\log (MAX\_SIZE / 64))$
- `rank`: $O(\log (MAX\_SIZE / 64))$
- `select`: `rank` に追加で二分探索
