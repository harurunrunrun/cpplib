---
title: Two Maximum Segment Tree (上位二値セグメント木) [KGSS]
documentation_of: ../src/structure/segtree/two_maximum_segtree.hpp
---

列の一点変更と、半開区間に含まれる最大値および二番目に大きい値の和を扱う。
同じ値が複数あれば、それぞれを別の要素として数える。

# `TwoMaximumAggregate<T>`

```cpp
template<class T>
struct TwoMaximumAggregate {
    array<T, 2> value;
    int count;
};
```

部分列の上位二値を降順に保持する内部集約値。`count` は保持している値の個数で
`0` 以上 `2` 以下である。構築、比較、field参照は $O(1)$ 時間である。

# コンストラクタ

```cpp
(1) TwoMaximumSegtree<T, MAX_SIZE>(int n = MAX_SIZE)
(2) TwoMaximumSegtree<T, MAX_SIZE>(const vector<T>& values)
```

1. 長さ `n`、各要素が `T{}` の列を構築する。
2. `values` を初期列として構築する。

時間計算量は $O(\mathtt{MAX\_SIZE})$、オブジェクトが保持する空間は
$O(\mathtt{MAX\_SIZE})$ である。

# `size`

```cpp
int size() const
```

列長を返す。時間計算量と追加空間計算量は $O(1)$。

# `set`

```cpp
void set(int position, const T& value)
```

`position` 番目を `value` に変更する。時間計算量は $O(\log n)$、追加空間計算量は
$O(1)$。

# `sum_of_two_largest`

```cpp
T sum_of_two_largest(int left, int right) const
```

半開区間 `[left, right)` に含まれる最大要素と二番目に大きい要素の和を返す。
時間計算量は $O(\log n)$、追加空間計算量は $O(1)$。

## 注意点

- `0 <= n <= MAX_SIZE` が必要である。
- `set` は `0 <= position < n`、区間queryは
  `0 <= left < right <= n` かつ `right-left >= 2` を要求する。
- 容量、添字または区間の違反では `runtime_error` を送出する。
- `T` はdefault構築、copy、`operator<`、`operator+` に対応する必要がある。
- 返値の加算結果は `T` で表現できなければならない。
