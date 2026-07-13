---
title: Dynamic Fully Indexable Dictionary (動的完備辞書)
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

# 計算量（公開操作別）

$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$ とする。

- int constructor (`DynamicFullyIndexableDictionary(int)`): $O(W)$
- vector constructor (`DynamicFullyIndexableDictionary(vector<bool>)`): $O(W+N\log(W+1))$
- `size`, `operator[]`: $O(1)$
- `set`, `flip`, `rank`: $O(\log(W+1))$
- `select`: $O(\log N\log(W+1))$

## 注意点

```cpp
DynamicFullyIndexableDictionary(int n = 0)
DynamicFullyIndexableDictionary(const vector<bool>& values)
int size() const
bool operator[](int k) const
```

`set/flip` は直ちにrank/selectへ反映される。`rank` は半開区間、`select` の出現番号は
0-indexedで、存在しなければ `size()`。長さ容量、点・区間、負の出現番号の違反では
`runtime_error`。各APIの計算量は上記表の通り。
