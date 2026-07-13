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

# API契約

```cpp
int size() const
int versions() const
int latest_version() const
bool get(int version, int k) const
int set(int k, bool value)
int flip(int k)
int rank(int version, bool value, int l, int r) const
int select(int version, bool value, int k) const
```

更新は最新版から新versionを作り、queryは任意の有効versionを読む。`rank` は半開区間、
`select` は0-indexedで該当なしなら `size()`。不正なversion・点・区間・出現番号、
version/node容量超過では `runtime_error`。各APIの計算量は上記表の通り。

# Constructor signature

```cpp
PartiallyPersistentFullyIndexableDictionary(int n = 0)
PartiallyPersistentFullyIndexableDictionary(const vector<bool>& values)
```
