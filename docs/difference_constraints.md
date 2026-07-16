---
title: Difference Constraints (差分制約)
documentation_of: ../src/algorithm/graph/difference_constraints.hpp
---

差分制約だけからなる線形計画問題。

```text
d_to - d_from <= bound
```

制約をすべて満たす中で `d_variable - d_reference` を最大化する。

制約 `d_to - d_from <= bound` は、`from -> to` に重み `bound` の辺を張ると最短路問題になる。

## API

```cpp
DifferenceConstraint<T>{from, to, bound}
void add_difference_at_most(constraints, from, to, bound)
void add_difference_at_least(constraints, from, to, bound)
void add_difference_equal(constraints, from, to, bound)
```

`add_difference_at_most` は `d_to - d_from <= bound` を追加する。

`add_difference_at_least` は `d_to - d_from >= bound` を追加する。

`add_difference_equal` は `d_to - d_from == bound` を追加する。

## 全変数の上界

```cpp
solve_difference_constraints<T>(n, reference, constraints, inf)
```

`reference` を基準に、各 `v` の `d_v - d_reference` の最大値を求める。

`DifferenceConstraintsResult<T>` を返す。

```cpp
bool feasible;
vector<T> maximum;
vector<char> bounded;
```

負閉路があり制約を満たす解がない場合 `feasible == false`。

`bounded[v] == true` のとき `maximum[v]` が `d_v - d_reference` の最大値。

`bounded[v] == false` の要素では `maximum[v]` の値は未定義。`inf` はその初期値にだけ使われる。

## 単一差分の上界

```cpp
maximum_difference<T>(n, reference, variable, constraints, inf)
```

`d_variable - d_reference` の最大値を求める。

`bounded == false` のとき上に非有界。

## 差分の範囲

```cpp
difference_range<T>(n, reference, variable, constraints, inf)
```

`d_variable - d_reference` の取りうる範囲を求める。

`has_minimum == false` のとき下に非有界。

`has_maximum == false` のとき上に非有界。

差分制約の経路和は `T` の表現範囲内である必要がある。

## 時間計算量

$M$ を現在の制約数とする。

- `add_difference_at_most`: 償却 $O(1)$
- `add_difference_at_least`: 償却 $O(1)$
- `add_difference_equal`: 償却 $O(1)$（制約を2本追加）
- `solve_difference_constraints`: $O(NM+N)$。実行可能性判定と `maximum` / `bounded` の長さ $N$ の出力を含む
- `has_finite_difference_upper_bound`: $O(1)$
- `maximum_difference`: $O(NM+N)$
- `difference_range`: $O(NM+N)$（`maximum_difference` を2回実行）

## 空間計算量

- `solve_difference_constraints`: 戻り値を含めて $O(N)$
- 単一目的・範囲関数も内部結果を含めて $O(N)$

## 注意点

`DifferenceUpperBoundResult` は `feasible`, `bounded`, `value` を持ち、両flagがtrueのときだけvalueが有効。`DifferenceRangeResult` は `feasible`, `has_minimum`, `has_maximum`, `minimum`, `maximum` を持ち、各端点は対応flagがtrueのときだけ有効。`has_finite_difference_upper_bound` は実行不能ならfalseを返し、variableを範囲検査する。

変数indexと制約の両端点は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
