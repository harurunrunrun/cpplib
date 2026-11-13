---
title: Matroid Partition (マトロイド分割)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/matroid_partition.hpp
---

各要素を高々 1 つのグループに割り当て、各グループがそれぞれ別のマトロイドで独立になるようにしながら、割り当てられる要素数を最大化する。名前空間は `combinatorial_optimization` である。

## `MatroidPartitionResult`

`groups[i]` は第 $i$ マトロイドに割り当てた独立集合、`uncovered` はどのグループにも割り当てられなかった要素である。

## `matroid_partition(ground_set_size, independent)`

`independent` は独立性オラクルの `std::vector` で、`independent[i]` は第 $i$ グループの独立性オラクルである。各オラクルは `std::vector<std::size_t>` を受け取り、独立かどうかを返す。

要素とグループの組ごとにコピーを作り、「各要素のコピーを高々 1 つ選ぶ」分割マトロイドと、各グループのマトロイドの直和との交差（`matroid_intersection`）に帰着する。

## 時間計算量

台集合の大きさを $n$、グループ数を $k$、コピーの台集合の大きさを $N = nk$、最終的に割り当てられた要素数を $r$、元のオラクル 1 回の時間を $T$ とする。交差の各段階で、現在の集合は両側で独立なので、1 回の交換判定では追加されるコピーの属するグループのオラクルだけを 1 回呼べばよい（マトロイドの遺伝性による）。引数の列は $O(r)$ 時間で作るため、時間計算量は $O(r N^2 (T + r))$ である。

## 空間計算量

$O(N + n + k)$。

## 注意点

- 各オラクルはマトロイドの公理を満たす必要がある（交換判定では変化したグループのオラクルしか呼ばない）。
- マトロイドが $0$ 個なら、全要素を `uncovered` として返す。
- コピーの個数 $nk$ が `std::size_t` で表せない場合は `std::length_error` を送出する。
- いずれかのオラクルが空集合を拒否した場合は `std::invalid_argument` を送出する。
