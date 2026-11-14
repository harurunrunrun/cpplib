---
title: Fully Indexable Dictionary (完備辞書)
documentation_of: ../../../../src/structure/wavelet_matrix/static/fully_indexable_dictionary.hpp
---

構築後に変更しない bit 列に対する rank / select 辞書。

## テンプレート引数

```cpp
FullyIndexableDictionary<MAX_SIZE = -1>
```

- `MAX_SIZE`: bit 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。

```cpp
using Variable = FullyIndexableDictionary<>;
using Fixed = FullyIndexableDictionary<N>;
```

## コンストラクタ

```cpp
(1) explicit FullyIndexableDictionary(int n = 0)
(2) explicit FullyIndexableDictionary(const vector<bool>& v)
```

1. 長さ `n` の全 0 の bit 列を作る。`set` で値を設定した後に `build` を呼ぶ。
2. `v` から構築する（`build` まで行う）。

## set / build

```cpp
void set(int k, bool value = true)
void build()
```

`set` は `k` 番目の bit を `value` にする。`build` は rank 用の累積和を構築する。`build` 後に `set` した場合は、再度 `build` が必要である。

## rank

```cpp
int rank(bool value, int r) const
int rank(bool value, int l, int r) const
```

区間 $[0, r)$ または $[l, r)$ にある `value` の個数を返す。

## select

```cpp
int select(bool value, int k) const
```

`value` が `k` 回目（0-indexed）に現れる位置を返す。存在しない場合は `size()` を返す。

## その他

```cpp
int size() const
bool operator[](int k) const
```

`size` は bit 列長を、`operator[]` は `k` 番目の bit を返す。

## 時間計算量

$N$ を bit 列長とし、$W$ を固定長版では $\lceil \texttt{MAX_SIZE} / 64 \rceil$、可変長版では $\lceil N / 64 \rceil$ とする。

- コンストラクタ (1): 配列の初期化を含めて $O(W)$
- コンストラクタ (2): $O(W + N)$
- `set`, `operator[]`, `rank`, `size`: $O(1)$
- `build`: $O(W)$
- `select`: $O(\log(\lceil N / 64 \rceil + 1) + 64)$

## 空間計算量

$O(W)$ ワード。

## 注意点

- `rank` / `select` の前に `build()` が必要で、`set` の後は再度 `build()` する。
- 長さは `MAX_SIZE` 以下（固定長版）、点と区間は列内、`select` の `k` は非負でなければならない。違反時は `std::runtime_error` を送出する。
