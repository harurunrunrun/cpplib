---
title: Offline Dynamic Component Aggregate (動的連結成分の可換モノイド集約) [dynamic_graph_vertex_add_component_sum]
documentation_of: ../../../../src/structure/graph/dynamic_connectivity/offline_dynamic_component_aggregate.hpp
---

辺の追加・削除、頂点値への可換モノイドの作用、連結成分の集約をオフラインで処理する。
辺の生存区間と頂点の更新を時間軸上のセグメント木に載せ、union by size と変更履歴による巻き戻しで処理する。

## API

```cpp
template<auto Monoid>
vector<S> offline_dynamic_component_aggregate(
    const vector<S>& initial,
    const vector<DynamicComponentQuery<Monoid>>& queries
);

template<auto Monoid>
struct DynamicComponentQuery{
    int type, first, second = -1;
    S value = Monoid.e();
};
```

`initial[v]` が頂点 $v$ の初期値で、頂点数は `initial.size()` である。各クエリの `type` は次の意味を持つ。

- `0`: 辺 `(first, second)` を追加する
- `1`: 辺 `(first, second)` を 1 本削除する
- `2`: 頂点 `first` の値を `Monoid.op(old, value)` に更新する
- `3`: 頂点 `first` を含む連結成分の値の集約を問い合わせる

戻り値は種類 `3` のクエリに対する答えを、クエリ順に並べたものである。平行辺と自己ループも扱える。

## 時間計算量

モノイド演算を定数時間とし、頂点数を $N$、クエリ数を $Q$ とする。

$O(N + Q\log(Q+1)\log(N+1))$。

## 空間計算量

$O(N+Q\log(Q+1))$。

## 注意点

- `Monoid` は既存の `Monoid` と同じく `S`、`op`、`e` を持つ定数（非型テンプレート引数）である。演算は結合的かつ可換でなければならない。逆元は不要なので、合成数を法とする積や最大値なども使える。
- `type` が $[0, 3]$ の範囲外、`first` が範囲外、または存在しない辺の削除は `std::invalid_argument` を、辺の `second` が範囲外の場合は `std::out_of_range` を送出する。
- 型の演算自体のオーバーフローは、呼び出し側で避ける。
