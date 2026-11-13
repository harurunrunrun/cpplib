---
title: Residue Count Range Tree (剰余個数区間木) [MULTQ3]
documentation_of: ../../../../src/structure/segtree/specialized/residue_count_range_tree.hpp
---

整数列に対して、区間の一様インクリメントと、指定した剰余を持つ要素数の区間クエリを扱う。内部では、剰余ごとの個数を集約する遅延セグメント木（`LazySegtree`）を用いる。`MAX_SIZE` を省略すると可変長版になり、長さの上限がなくなる。

## コンストラクタ

```cpp
(1) ResidueCountRangeTree<Count, MOD, MAX_SIZE>(int size = MAX_SIZE)  // 可変長版の既定値は 0
(2) ResidueCountRangeTree<Count, MOD, MAX_SIZE>(
        const vector<long long>& values
    )
```

1. 長さ `size`、各要素が 0 の列を構築する。
2. `values` を初期列として構築する。各値は法 `MOD` での剰余だけを保持する。

時間計算量は $O(\texttt{MAX_SIZE} \cdot \mathrm{MOD})$ で、オブジェクトが保持する空間も $O(\texttt{MAX_SIZE} \cdot \mathrm{MOD})$ である。

## `size`

```cpp
int size() const
```

列の長さを返す。時間計算量と追加の空間計算量は $O(1)$ である。

## `increment`

```cpp
void increment(int left, int right)
```

半開区間 $[left, right)$ の各要素に 1 を加える。列の長さを $N$ として、時間計算量は $O(\mathrm{MOD}\log N)$、追加の空間計算量は $O(\mathrm{MOD})$ である。

## `count_residue`

```cpp
Count count_residue(int left, int right, long long residue)
```

半開区間 $[left, right)$ にある、値が `residue` と合同（法 `MOD`）な要素の個数を返す。`residue` は数学的な剰余（$[0, \mathrm{MOD})$）に正規化する。時間計算量は $O(\mathrm{MOD}\log N)$、追加の空間計算量は $O(\mathrm{MOD})$ である。遅延作用を押し下げるため `const` ではない。

## 注意点

- `MOD > 0`（`static_assert`）と、固定容量版では `0 <= size <= MAX_SIZE` が必要である。
- 区間は `0 <= left <= right <= size()` を満たす半開区間で指定する。
- 容量または区間の違反では `std::runtime_error` を送出する。
- `Count` は 0 と 1 からの構築・コピー・`+=` に対応し、個数を表現できる必要がある。
- 各要素の値そのものは保持せず、法 `MOD` での剰余だけを保持する。
