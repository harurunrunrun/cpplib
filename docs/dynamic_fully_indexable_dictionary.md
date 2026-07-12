---
title: Dynamic Fully Indexable Dictionary
documentation_of: ../src/structure/wavelet_matrix/dynamic_fully_indexable_dictionary.hpp
---

bitの変更に対応したrank/select辞書。

# テンプレート引数

```cpp
DynamicFullyIndexableDictionary<MAX_SIZE>
```

# 更新

```cpp
void set(int k, bool value)
void flip(int k)
```

`set` は `k` 番目を `value` にし、`flip` は反転する。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# rank / select

```cpp
int rank(bool value, int r) const
int rank(bool value, int l, int r) const
int select(bool value, int k) const
```

`rank` は指定区間にある `value` の個数を返す。`select` は `value` が `k` 回目に現れる位置を返し、存在しない場合は `size()` を返す。`k` は0-indexed。

## 時間計算量

- `rank`: $O(\log \mathrm{MAX\_SIZE})$
- `select`: $O(\log^2 \mathrm{MAX\_SIZE})$
