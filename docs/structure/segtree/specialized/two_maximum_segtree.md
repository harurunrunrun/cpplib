---
title: Two Maximum Segment Tree (上位二値セグメント木) [KGSS]
documentation_of: ../../../../src/structure/segtree/specialized/two_maximum_segtree.hpp
---

列の一点変更と、半開区間に含まれる最大値および二番目に大きい値の和を扱う。
同じ値が複数あれば、それぞれを別の要素として数える。

## `TwoMaximumAggregate<T>`

```cpp
template<class T>
struct TwoMaximumAggregate {
    array<T, 2> value;
    int count;
};
```

部分列の上位二値を降順に保持する内部集約値。`count` は保持している値の個数で
`0` 以上 `2` 以下である。構築、比較、field参照は $O(1)$ 時間である。

## コンストラクタ

```cpp
(1) TwoMaximumSegtree<T, MAX_SIZE>(int n = MAX_SIZE)
(2) TwoMaximumSegtree<T, MAX_SIZE>(const vector<T>& values)
```

1. 長さ `n`、各要素が `T{}` の列を構築する。
2. `values` を初期列として構築する。

時間計算量は $O(\mathtt{MAX\_SIZE})$、オブジェクトが保持する空間は
$O(\mathtt{MAX\_SIZE})$ である。

## `size`

```cpp
int size() const
```

列長を返す。時間計算量と追加空間計算量は $O(1)$。

## `set`

```cpp
void set(int position, const T& value)
```

`position` 番目を `value` に変更する。時間計算量は $O(\log n)$、追加空間計算量は
$O(1)$。

## `sum_of_two_largest`

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

## 容量を省略する場合

容量引数の既定値は `-1`。`TwoMaximumSegtree<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
