---
title: Difference Constraints (差分制約、牛ゲー)
documentation_of: ../../../../../src/algorithm/graph/constraints/systems/difference_constraints.hpp
---

差分制約

$$
d_{\mathrm{to}} - d_{\mathrm{from}} \le \mathrm{bound}
$$

だけからなる線形計画問題を解き、すべての制約を満たす中での $d_{\mathrm{variable}} - d_{\mathrm{reference}}$ の最大値などを求める。

制約 $d_{\mathrm{to}} - d_{\mathrm{from}} \le \mathrm{bound}$ は、`from` から `to` へ重み `bound` の辺を張ると最短路問題に帰着する。

## 制約の追加

```cpp
template <class T>
struct DifferenceConstraint { int from; int to; T bound; };

void add_difference_at_most(std::vector<DifferenceConstraint<T>>& constraints, int from, int to, T bound)
void add_difference_at_least(std::vector<DifferenceConstraint<T>>& constraints, int from, int to, T bound)
void add_difference_equal(std::vector<DifferenceConstraint<T>>& constraints, int from, int to, T bound)
```

- `add_difference_at_most`: $d_{\mathrm{to}} - d_{\mathrm{from}} \le \mathrm{bound}$ を追加する。
- `add_difference_at_least`: $d_{\mathrm{to}} - d_{\mathrm{from}} \ge \mathrm{bound}$ を追加する（内部では `to` から `from` への重み $-\mathrm{bound}$ の辺になる）。
- `add_difference_equal`: $d_{\mathrm{to}} - d_{\mathrm{from}} = \mathrm{bound}$ を追加する（制約を 2 本追加する）。

整数型 `T` で `bound` の符号反転がオーバーフローする場合は `overflow_error` を送出する。

## 全変数の上界

```cpp
DifferenceConstraintsResult<T> solve_difference_constraints(
    int n, int reference,
    const std::vector<DifferenceConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
)
```

`reference` を基準として、各変数 `v` について $d_v - d_{\mathrm{reference}}$ の最大値を求める。戻り値の型は次のとおり。

```cpp
template <class T>
struct DifferenceConstraintsResult {
    bool feasible;
    std::vector<T> maximum;
    std::vector<char> bounded;
};
```

- 負閉路があり、制約を満たす解が存在しない場合は `feasible == false` となる（このとき `maximum`, `bounded` は空）。
- `bounded[v]` が真のとき、`maximum[v]` が $d_v - d_{\mathrm{reference}}$ の最大値である。
- `bounded[v]` が偽の変数は上に非有界であり、`maximum[v]` は初期値 `inf` のまま残る。`inf` はこの初期値にだけ使われる。

内部表現された全辺で `bound >= 0` なら負閉路は存在しないため、実行可能性判定を省略して Dijkstra 法で各上界を求める。この高速化の条件は、追加用関数で変換した後の辺に対して判定する。たとえば正の下界を追加する `add_difference_at_least` は負の辺を生成するため、条件を満たすとは限らない。

```cpp
bool has_finite_difference_upper_bound(const DifferenceConstraintsResult<T>& result, int variable)
```

実行不能なら `false`、そうでなければ `result.bounded[variable]` を返す。実行可能な場合は `variable` の範囲を検査する。

## 単一差分の上界

```cpp
DifferenceUpperBoundResult<T> maximum_difference(
    int n, int reference, int variable,
    const std::vector<DifferenceConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
)
```

$d_{\mathrm{variable}} - d_{\mathrm{reference}}$ の最大値を求める。戻り値 `DifferenceUpperBoundResult<T>` はメンバ `feasible`, `bounded`, `value` をもち、`value` は `feasible` と `bounded` がともに真のときだけ有効である。`bounded == false` なら上に非有界である。

## 差分の範囲

```cpp
DifferenceRangeResult<T> difference_range(
    int n, int reference, int variable,
    const std::vector<DifferenceConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
)
```

$d_{\mathrm{variable}} - d_{\mathrm{reference}}$ の取りうる範囲を求める。戻り値 `DifferenceRangeResult<T>` はメンバ `feasible`, `has_minimum`, `has_maximum`, `minimum`, `maximum` をもち、各端点は対応するフラグが真のときだけ有効である。

- `has_minimum == false` なら下に非有界である。
- `has_maximum == false` なら上に非有界である。

## 時間計算量

$N$ を変数の個数、$M$ を現在の制約の本数とする。「非負辺のみ」とは、内部表現されたすべての制約で `bound >= 0` である場合を指す。

- `add_difference_at_most`, `add_difference_at_least`, `add_difference_equal`: 償却 $O(1)$
- `solve_difference_constraints`: 一般には Bellman–Ford 法により $O(NM + N)$、非負辺のみなら Dijkstra 法により $O((N + M) \log (N + M))$。実行可能性判定と長さ $N$ の `maximum`, `bounded` の出力を含む。
- `has_finite_difference_upper_bound`: $O(1)$
- `maximum_difference`: `solve_difference_constraints` と同じ
- `difference_range`: `solve_difference_constraints` と同じ（内部で `maximum_difference` を 2 回実行する）

## 空間計算量

いずれの関数も、戻り値と（非負辺のみの場合の）隣接リストを含めて $O(N + M)$。

## 注意点

- `n` が非負でない、`reference` や `variable` が $[0, n)$ の範囲外、または制約の端点が範囲外の場合は `runtime_error` を送出する。
- 整数型 `T` では、緩和で計算する和、または下界・下端の符号反転が `T` の表現範囲を超える場合に `overflow_error` を送出する。非整数型では、演算結果が `T` の有限な表現範囲に収まらなければならない。
