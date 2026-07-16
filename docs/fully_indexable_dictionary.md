---
title: Fully Indexable Dictionary (完備辞書)
documentation_of: ../src/structure/wavelet_matrix/fully_indexable_dictionary.hpp
---

固定bit列に対するrank/select辞書。

## テンプレート引数

```cpp
FullyIndexableDictionary<MAX_SIZE>
```

## コンストラクタ

```cpp
(1) FullyIndexableDictionary(int n = 0)
(2) FullyIndexableDictionary(const vector<bool>& v)
```

1. 長さ `n` の0列を作る。`set` の後に `build` を呼ぶ。
2. `v` から構築する。

## set / build

```cpp
void set(int k, bool value = true)
void build()
```

`k` 番目を設定し、rank用の累積和を構築する。`build` 後に `set` した場合は再度 `build` が必要。

## 時間計算量

- `set`: $O(1)$
- `build`: $O(\mathrm{MAX\_SIZE}/64)$

## rank

```cpp
int rank(bool value, int r) const
int rank(bool value, int l, int r) const
```

区間 $[0,r)$ または $[l,r)$ にある `value` の個数を返す。

## 時間計算量

- $O(1)$

## select

```cpp
int select(bool value, int k) const
```

`value` が `k` 回目に現れる位置を返す。存在しない場合は `size()` を返す。`k` は0-indexed。

## 時間計算量

- $O(\log (\lceil N/64 \rceil + 1) + 64)$

## 計算量（公開操作別）

$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$ とする。

- `FullyIndexableDictionary(int)`: 固定長配列の初期化を含めて $O(W)$
- `FullyIndexableDictionary(vector<bool>)`: $O(W+N)$
- `set`, `operator[]`, `rank`, `size`: $O(1)$
- `build`: $O(W)$
- `select`: $O(\log(\lceil N/64\rceil+1)+64)$

## 注意点

```cpp
int size() const
bool operator[](int k) const
```

`size` はbit列長、`operator[]` は `k` 番目を返す。`rank/select` の前に
`build()` が必要で、`set` 後は再度buildする。長さは `MAX_SIZE` 以下、
点と区間は列内、`select` の `k` は非負でなければならない。
違反時は `runtime_error`。各APIの計算量は上記表の通り。
