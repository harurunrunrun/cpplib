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

# 計算量（公開操作別）

$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$、$H=\lceil\log_2\max(1,W)\rceil$ とする。

- int constructor: $O(W+\mathtt{MAX\_VERSION}(H+1))$
- vector constructor: $O(W+\mathtt{MAX\_VERSION}(H+1)+N)$
- `size`, `versions`, `latest_version`: $O(1)$
- `get`, `set`, `flip`, `rank`: $O(H+1)$
- `select`: $O((H+1)\log N)$

`set`と`flip`は常に最新versionから新versionを作り、根から葉まで $O(H+1)$ 個のnodeを複製する。
