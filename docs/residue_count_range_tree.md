---
title: Residue Count Range Tree (剰余個数区間木) [MULTQ3]
documentation_of: ../src/structure/segtree/residue_count_range_tree.hpp
---

整数列の区間一様インクリメントと、指定した剰余を持つ要素数の区間queryを扱う。
内部では剰余別個数を集約するlazy segment treeを用いる。

## コンストラクタ

```cpp
(1) ResidueCountRangeTree<Count, MOD, MAX_SIZE>(int size = MAX_SIZE)
(2) ResidueCountRangeTree<Count, MOD, MAX_SIZE>(
        const vector<long long>& values
    )
```

1. 長さ `size`、各要素が0の列を構築する。
2. `values` を初期列として構築する。各値は法 `MOD` の剰余だけを保持する。

時間計算量は $O(\mathtt{MAX\_SIZE}\,MOD)$、オブジェクトが保持する空間は
$O(\mathtt{MAX\_SIZE}\,MOD)$ である。

## `size`

```cpp
int size() const
```

列長を返す。時間計算量と追加空間計算量は $O(1)$。

## `increment`

```cpp
void increment(int left, int right)
```

半開区間 `[left, right)` の各要素へ1を加える。時間計算量は
$O(MOD\log n)$、追加空間計算量は $O(MOD)$。

## `count_residue`

```cpp
Count count_residue(int left, int right, long long residue)
```

半開区間 `[left, right)` にある、値が `residue` と合同な要素数を返す。
`residue` は数学的なmodで正規化する。時間計算量は $O(MOD\log n)$、
追加空間計算量は $O(MOD)$。

## 注意点

- `MOD > 0` と `0 <= size <= MAX_SIZE` が必要である。
- 区間は `0 <= left <= right <= size()` を満たす半開区間で指定する。
- 容量または区間の違反では `runtime_error` を送出する。
- `Count` は0と1からの構築、copy、`+=` に対応し、個数を表現できる必要がある。
- 各要素の値そのものは保持せず、法 `MOD` の剰余だけを保持する。
