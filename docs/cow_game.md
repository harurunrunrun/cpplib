---
title: Cow Game
documentation_of: ../src/algorithm/graph/cow_game.hpp
---

差分制約だけからなる線形計画問題。

```text
d_to - d_from <= cost
```

制約をすべて満たす中で `d_target - d_source` を最大化する。

制約 `d_to - d_from <= cost` は、`from -> to` に重み `cost` の辺を張ると最短路問題になる。

# constraint

```cpp
CowGameConstraint<T>{from, to, cost}
void cow_game_add_at_most(constraints, from, to, cost)
void cow_game_add_at_least(constraints, from, to, cost)
void cow_game_add_equal(constraints, from, to, cost)
```

`cow_game_add_at_most` は `d_to - d_from <= cost` を追加する。

`cow_game_add_at_least` は `d_to - d_from >= cost` を追加する。

`cow_game_add_equal` は `d_to - d_from == cost` を追加する。

# all maximum

```cpp
cow_game<T>(n, source, constraints, inf)
```

`source` を基準に、各 `v` の `d_v - d_source` の最大値を求める。

`CowGameResult<T>` を返す。

```cpp
bool feasible;
vector<T> maximum;
vector<char> bounded;
```

負閉路があり制約を満たす解がない場合 `feasible == false`。

`bounded[v] == true` のとき `maximum[v]` が `d_v - d_source` の最大値。

`bounded[v] == false` の要素では `maximum[v]` の値は未定義。`inf` はその初期値にだけ使われる。

# single objective

```cpp
cow_game_max_difference<T>(n, source, target, constraints, inf)
```

`d_target - d_source` の最大値を求める。

`bounded == false` のとき上に非有界。

# range

```cpp
cow_game_difference_range<T>(n, source, target, constraints, inf)
```

`d_target - d_source` の取りうる範囲を求める。

`has_minimum == false` のとき下に非有界。

`has_maximum == false` のとき上に非有界。

差分制約の経路和は `T` の表現範囲内である必要がある。

## 時間計算量

$M$ を現在の制約数とする。

- `cow_game_add_at_most`: 償却 $O(1)$
- `cow_game_add_at_least`: 償却 $O(1)$
- `cow_game_add_equal`: 償却 $O(1)$（制約を2本追加）
- `cow_game`: $O(NM+N)$。実行可能性判定と `maximum` / `bounded` の長さ $N$ の出力を含む
- `cow_game_has_maximum`: $O(1)$
- `cow_game_max_difference`: $O(NM+N)$
- `cow_game_difference_range`: $O(NM+N)$（`cow_game_max_difference` を2回実行）

## 空間計算量

- `cow_game`: 戻り値を含めて $O(N)$
- 単一目的・範囲関数も内部結果を含めて $O(N)$

## API契約・前提・例外

`CowGameMaximumResult` は `feasible`, `bounded`, `value` を持ち、両flagがtrueのときだけvalueが有効。`CowGameRangeResult` は `feasible`, `has_minimum`, `has_maximum`, `minimum`, `maximum` を持ち、各端点は対応flagがtrueのときだけ有効。`cow_game_has_maximum` は実行不能ならfalseを返し、targetを範囲検査する。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
