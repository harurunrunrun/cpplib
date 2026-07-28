---
title: Matroid Union (マトロイド和)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/matroid_union.hpp
---

1つのマトロイドの独立集合 $k$ 個の和として表せる最大部分集合と、その分割を求める。
現在の分割に対する交換グラフを幅優先探索し、被覆要素数を1増やす増加路を反転する。

## API

```cpp
struct MatroidUnionResult {
    vector<vector<size_t>> groups;
    vector<size_t> uncovered;
};
```

- `groups[i]`: 第 $i$ copy に割り当てた独立集合
- `uncovered`: どの copy にも割り当てられなかった要素

```cpp
template<class IndependenceOracle>
MatroidUnionResult matroid_union(
    size_t ground_set_size,
    size_t copy_count,
    IndependenceOracle independent
);
```

`independent(elements)` は `vector<size_t>` が元のマトロイドで独立なら
`true` を返す。返り値の全 group は独立で、全要素はちょうど1つの group または
`uncovered` に現れる。

## 時間計算量

$n=ground_set_size$、$k=copy_count$、最終被覆数を $r$、
1回の oracle 呼び出しを $T$ とする。

- 1回の増加路探索: oracle 呼び出し $O(n(k+n))$ 回
- ベクトル候補の構築も含む1回の探索:
  $O(n^3+n(k+n)T)$
- `matroid_union` 全体:
  $O((r+1)(n^3+n(k+n)T))$
- 追加空間: $O(n+k)$（oracle に渡す一時ベクトルを含む）

## 注意点

同一要素は高々1 groupに入る。`copy_count == 0` では
`groups` は空で、全要素を `uncovered` として返す。
`copy_count` が `groups.max_size()` を超える場合は `length_error` を送出する。
oracle はマトロイドの独立集合族を表し、呼び出し中に保持されない
`vector<size_t>` を受け取れる必要がある。
