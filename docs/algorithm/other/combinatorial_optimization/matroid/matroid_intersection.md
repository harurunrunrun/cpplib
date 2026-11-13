---
title: Matroid Intersection (マトロイド交差)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/matroid_intersection.hpp
---

同じ台集合上の 2 つのマトロイドに共通して独立な集合のうち、要素数最大のものを、交換グラフ上の最短増加路（幅優先探索）で求める。名前空間は `combinatorial_optimization` である。

## `MatroidIntersectionResult`

`elements` が最大共通独立集合（要素番号の昇順）。要素番号は台集合の大きさ `ground_set_size` を $n$ として $[0, n)$ の範囲である。

## `matroid_intersection(ground_set_size, first_independent, second_independent)`

各独立性オラクルは、要素番号を昇順に並べた `std::vector<std::size_t>` を受け取り、その集合が独立かどうかを返す。

## 時間計算量

最大共通独立集合の大きさを $r$、台集合の大きさを $n$、オラクル 1 回の時間を $T$ とする。1 回の増加路探索でオラクルを $O(n^2)$ 回呼ぶ。現在の共通独立集合の昇順列を探索ごとに 1 回作り、各呼び出しの引数はそこから 1 要素の除去と挿入で $O(r)$ 時間で構築するため、全体で $O(r n^2 (T + r))$ 時間である。

## 空間計算量

$O(n)$（オラクルに渡す一時的な列を含む）。

## 注意点

オラクルはマトロイドの公理を満たし、空集合を独立と判定する必要がある。空集合を拒否した場合は `std::invalid_argument` を送出する。
