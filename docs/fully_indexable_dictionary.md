---
title: Fully Indexable Dictionary
documentation_of: ../src/structure/wavelet_matrix/fully_indexable_dictionary.hpp
---

固定bit列に対するrank/select辞書。

# テンプレート引数

```cpp
FullyIndexableDictionary<MAX_SIZE>
```

# コンストラクタ

```cpp
(1) FullyIndexableDictionary(int n = 0)
(2) FullyIndexableDictionary(const vector<bool>& v)
```

1. 長さ `n` の0列を作る。`set` の後に `build` を呼ぶ。
2. `v` から構築する。

# set / build

```cpp
void set(int k, bool value = true)
void build()
```

`k` 番目を設定し、rank用の累積和を構築する。`build` 後に `set` した場合は再度 `build` が必要。

## 時間計算量

- `set`: $O(1)$
- `build`: $O(\mathrm{MAX\_SIZE}/64)$

# rank

```cpp
int rank(bool value, int r) const
int rank(bool value, int l, int r) const
```

区間 $[0,r)$ または $[l,r)$ にある `value` の個数を返す。

## 時間計算量

- $O(1)$

# select

```cpp
int select(bool value, int k) const
```

`value` が `k` 回目に現れる位置を返す。存在しない場合は `size()` を返す。`k` は0-indexed。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$
