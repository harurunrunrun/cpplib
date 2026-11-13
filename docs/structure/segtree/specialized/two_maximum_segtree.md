---
title: Two Maximum Segment Tree (上位二値セグメント木) [KGSS]
documentation_of: ../../../../src/structure/segtree/specialized/two_maximum_segtree.hpp
---

列の一点変更と、半開区間に含まれる最大値と二番目に大きい値の和を求めるクエリを扱う。
同じ値が複数あれば、それぞれを別の要素として数える。

## `TwoMaximumAggregate<T>`

```cpp
template<class T>
struct TwoMaximumAggregate {
    array<T, 2> value;
    int count;
};
```

部分列の上位二値を降順に保持する内部集約値である。`count` は保持している値の個数で、
`0` 以上 `2` 以下である。構築、比較（`operator==`）、メンバ参照は $O(1)$ 時間である。

## コンストラクタ

```cpp
(1) TwoMaximumSegtree<T, MAX_SIZE>(int n = MAX_SIZE)
(2) TwoMaximumSegtree<T, MAX_SIZE>(const vector<T>& values)
```

1. 長さ `n`、各要素が `T{}` の列を構築する。可変長版（`MAX_SIZE` 省略時）では `n` のデフォルト値は `0` である。
2. `values` を初期列として構築する。

固定長版の時間計算量は $O(\texttt{MAX_SIZE})$、オブジェクトが保持する空間は
$O(\texttt{MAX_SIZE})$ である。可変長版ではどちらも $O(n)$ である。

## `size`

```cpp
int size() const
```

列の長さを返す。時間計算量と追加空間計算量は $O(1)$ である。

## `set`

```cpp
void set(int position, const T& value)
```

`position` 番目の要素を `value` に変更する。時間計算量は $O(\log n)$、追加空間計算量は
$O(1)$ である。

## `sum_of_two_largest`

```cpp
T sum_of_two_largest(int left, int right) const
```

半開区間 `[left, right)` に含まれる最大の要素と二番目に大きい要素の和を返す。
時間計算量は $O(\log n)$、追加空間計算量は $O(1)$ である。

## 容量を省略する場合

容量引数のデフォルト値は `-1` である。`TwoMaximumSegtree<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使う。非負の容量を指定する固定配列版も引き続き利用できる。

可変長版は実行時の要素数に比例する記憶領域を確保し、固定配列版と同じ更新・取得アルゴリズムを用いる。整数添字の表現範囲と実メモリ容量が上限となる。

## 注意点

- 固定長版では `0 <= n <= MAX_SIZE` が必要である。
- `set` は `0 <= position < n` を、区間クエリは
  `0 <= left < right <= n` かつ `right - left >= 2` を要求する。
- 容量・添字・区間の違反では `runtime_error` を送出する。
- `T` はデフォルト構築、コピー、`operator<`、`operator+` に対応する必要がある。
- 戻り値の加算結果は `T` で表現できなければならない。
