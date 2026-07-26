---
title: Dynamic Fully Indexable Dictionary (動的完備辞書)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_fully_indexable_dictionary.hpp
---

bit列への挿入・削除・置換と、rank/selectを扱う動的完備辞書。
固定容量の葉を持つ決定的な平衡 B+ 木に長さと1の個数を保持するため、
挿入・削除時に後続要素全体を移動したり列全体を再構築したりしない。

## 構築と取得

```cpp
DynamicFullyIndexableDictionary(int n = 0)
DynamicFullyIndexableDictionary(const vector<bool>& values)
DynamicFullyIndexableDictionary(const DynamicFullyIndexableDictionary&)
DynamicFullyIndexableDictionary(DynamicFullyIndexableDictionary&&)
DynamicFullyIndexableDictionary& operator=(const DynamicFullyIndexableDictionary&)
DynamicFullyIndexableDictionary& operator=(DynamicFullyIndexableDictionary&&)
int size() const
bool operator[](int k) const
```

長さは常に `MAX_SIZE` 以下でなければならない。整数だけを渡す構築では、
長さ `n` の0列を作る。

## 更新

```cpp
void set(int k, bool value)
void flip(int k)
void insert(int position, bool value)
bool erase(int position)
void push_back(bool value)
bool pop_back()
```

`insert` は `position` の直前へ挿入する。`erase` と `pop_back` は
削除したbitを返す。

## rank / select

```cpp
int rank(bool value, int r) const
int rank(bool value, int l, int r) const
int select(bool value, int occurrence) const
```

`rank` は半開区間にある `value` の個数を返す。`select` は
`occurrence` 番目の位置を返し、存在しなければ `size()` を返す。
出現番号は0-indexed。

## 時間計算量

現在の列長を $N$ とする。次は決定的な B+ 木による worst-case 計算量である。

- vector constructor: $O(N)$
- int constructor: $O(N)$
- コピー構築・コピー代入: $O(N)$
- ムーブ構築・ムーブ代入: $O(1)$
- `size`: $O(1)$
- `operator[]`, `set`, `flip`: $O(\log N)$
- `insert`, `erase`, `push_back`, `pop_back`: $O(\log N)$
- `rank`, `select`: $O(\log N)$
- 空間計算量: $O(N)$

## 注意点

添字・出現番号は0-indexed、区間は半開区間。`MAX_SIZE`、添字、区間、
負の出現番号の違反では `runtime_error` を送出する。
