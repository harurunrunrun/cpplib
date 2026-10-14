---
title: Offline Dynamic Component Aggregate (動的連結成分の可換モノイド集約) [dynamic_graph_vertex_add_component_sum]
documentation_of: ../../../../src/structure/graph/dynamic_connectivity/offline_dynamic_component_aggregate.hpp
---

`offline_dynamic_component_aggregate<Monoid>(initial,queries)` は辺の追加・削除、頂点値への可換モノイド作用、連結成分の集約をオフラインで処理する。`DynamicComponentQuery<Monoid>` の `type` は0:辺追加、1:辺削除、2:頂点値を `Monoid.op(old,value)` に更新、3:成分の集約。返り値は種類3の問い合わせの順。平行辺と自己ループも扱う。

## 時間計算量

$O((N+Q)\log(Q+1)\log(N+1))$、空間 $O(N+Q\log(Q+1))$。モノイド演算を定数時間とする。辺の生存区間と頂点更新を時間区間木に載せ、union-by-sizeと変更履歴で巻き戻す。

## 注意点

`Monoid` は既存の `Monoid` と同じ `S`, `op`, `e` を持つ構造的定数。演算は結合的・可換であること。逆元は不要なので、合成数法の積・最大値なども使える。削除対象の辺がない場合や不正な添字は例外。型の演算自体のオーバーフローは呼び出し側で避ける。
