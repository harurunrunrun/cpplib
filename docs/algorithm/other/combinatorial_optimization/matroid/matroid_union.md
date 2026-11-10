---
title: Matroid Union (マトロイド和)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/matroid_union.hpp
---

1 つのマトロイドの独立集合 $k$ 個の和として表せる最大の部分集合と、その分割を求める。
現在の分割に対する交換グラフを幅優先探索し、割り当てられる要素数を $1$ 増やす増加路に沿って割り当てを入れ替える。名前空間は `combinatorial_optimization` である。

## API

```cpp
struct MatroidUnionResult {
    std::vector<std::vector<std::size_t>> groups;
    std::vector<std::size_t> uncovered;
};
```

- `groups[i]`: 第 $i$ コピーに割り当てた独立集合
- `uncovered`: どのコピーにも割り当てられなかった要素

```cpp
template<class IndependenceOracle>
MatroidUnionResult matroid_union(
    std::size_t ground_set_size,
    std::size_t copy_count,
    IndependenceOracle independent
);
```

`independent(elements)` は、`std::vector<std::size_t>` で与えた集合が元のマトロイドで独立なら `true` を返す。戻り値のすべてのグループは独立であり、すべての要素はちょうど 1 つのグループまたは `uncovered` に現れる。

## 時間計算量

$n$ を `ground_set_size`、$k$ を `copy_count`、最終的に割り当てられた要素数を $r$、オラクル 1 回の呼び出しの時間を $T$ とする。

- 1 回の増加路探索: オラクル呼び出し $O(n(k+n))$ 回
- 候補の列の構築も含めた 1 回の探索: $O(n^3+n(k+n)T)$
- `matroid_union` 全体: $O((r+1)(n^3+n(k+n)T))$

## 空間計算量

追加領域 $O(n+k)$（オラクルに渡す一時的な列を含む）。

## 注意点

- 同じ要素は高々 1 つのグループに入る。
- `copy_count == 0` では `groups` は空で、全要素を `uncovered` として返す。
- `copy_count` が `groups.max_size()` を超える場合は `std::length_error` を送出する。
- オラクルはマトロイドの独立集合族を表し、呼び出しの間だけ有効な `std::vector<std::size_t>` を受け取れる必要がある（オラクル側で参照を保持してはならない）。
