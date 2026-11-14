---
title: Dynamic Fully Indexable Dictionary (動的完備辞書)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_fully_indexable_dictionary.hpp
---

bit 列への挿入・削除・置換と、rank / select を扱う動的完備辞書。固定容量の葉を持つ決定的な平衡 B+ 木の各ノードに長さと 1 の個数を保持するため、挿入・削除のたびに後続要素全体を移動したり、列全体を再構築したりしない。

## テンプレート引数

```cpp
DynamicFullyIndexableDictionary<MAX_SIZE = -1>
```

- `MAX_SIZE`: 列長の上限。省略時（`-1`）は上限なし（`int` の範囲内）。内部表現は B+ 木で、`MAX_SIZE` によって確保する領域は変わらない。

```cpp
using Variable = DynamicFullyIndexableDictionary<>;
using Fixed = DynamicFullyIndexableDictionary<N>;
```

## 構築と取得

```cpp
explicit DynamicFullyIndexableDictionary(int n = 0)
explicit DynamicFullyIndexableDictionary(const vector<bool>& values)
DynamicFullyIndexableDictionary(const DynamicFullyIndexableDictionary&)
DynamicFullyIndexableDictionary(DynamicFullyIndexableDictionary&&) noexcept
DynamicFullyIndexableDictionary& operator=(const DynamicFullyIndexableDictionary&)
DynamicFullyIndexableDictionary& operator=(DynamicFullyIndexableDictionary&&) noexcept
int size() const
bool operator[](int k) const
```

整数を渡すと長さ `n` の全 0 の列を、`values` を渡すとその列を作る。`size` は列長を、`operator[]` は `k` 番目の bit を返す。

## 更新

```cpp
void set(int k, bool value)
void flip(int k)
void insert(int position, bool value)
bool erase(int position)
void push_back(bool value)
bool pop_back()
```

`set` は `k` 番目を `value` にし、`flip` は反転する。`insert` は `position` の直前に挿入する（`position == size()` なら末尾）。`erase` と `pop_back` は削除した bit を返す。

## rank / select

```cpp
int rank(bool value, int r) const
int rank(bool value, int l, int r) const
int select(bool value, int occurrence) const
```

`rank` は区間 $[0, r)$ または $[l, r)$ にある `value` の個数を返す。`select` は `value` の `occurrence` 番目（0-indexed）の出現位置を返し、存在しなければ `size()` を返す。

## 時間計算量

現在の列長を $N$ とする。以下は決定的な B+ 木による最悪計算量である（葉の容量 128 などは定数とみなす）。

- コンストラクタ: $O(N)$
- コピー構築・コピー代入: $O(N)$
- ムーブ構築・ムーブ代入: $O(1)$
- `size`: $O(1)$
- `operator[]`, `set`, `flip`: $O(\log N)$
- `insert`, `erase`, `push_back`, `pop_back`: $O(\log N)$
- `rank`, `select`: $O(\log N)$

## 空間計算量

$O(N)$。

## 注意点

- 添字・出現番号は 0-indexed、区間は半開区間である。
- 列長が `MAX_SIZE`（省略時は `int` の最大値）を超える操作、範囲外の添字・区間、空列からの `pop_back`、負の出現番号では `std::runtime_error` を送出する。
