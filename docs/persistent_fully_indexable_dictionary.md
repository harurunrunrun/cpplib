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

# 計算量（公開操作別）

$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$、$H=\lceil\log_2\max(1,W)\rceil$ とする。

- int constructor: 固定容量のversion/root/node配列と初期木を含めて $O(W+\mathtt{MAX\_VERSION}(H+1))$
- vector constructor: $O(W+\mathtt{MAX\_VERSION}(H+1)+N)$
- `size`, `versions`, `latest_version`, `fork`: $O(1)$
- `get`, `set`, `flip`, `rank`: $O(H+1)$
- `select`: $O((H+1)\log N)$

`set`と`flip`は根から葉まで $O(H+1)$ 個のnodeを複製し、`fork`は根だけを共有する。
