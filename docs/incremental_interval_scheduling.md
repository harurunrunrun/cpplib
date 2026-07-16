---
title: Incremental Interval Scheduling (増分区間スケジューリング)
documentation_of: ../src/structure/other/incremental_interval_scheduling.hpp
---

整数座標上の半開区間を任意順に追加し、互いに重ならない区間を選べる最大個数を各時点で保持する。
包含関係にある区間の外側は解を悪化させず除去できるため、極小区間だけを始点順・終点順がともに単調な集合として管理する。
各整数座標 `s` から、区間がなければ `s + 1` へ重み `0`、極小区間 `[s,t)` があれば `t` へ重み `1` の遷移を張る。
この遷移森を Link-Cut Tree で管理し、最小座標から根までのパス和を答えとする。

## 型

```cpp
IncrementalIntervalScheduling<Coordinate>
```

`Coordinate` は `bool` 以外の整数型とする。

### `Interval`

```cpp
struct Interval {
    Coordinate start;
    Coordinate finish;
    size_t id;
};
```

追加された半開区間 `[start, finish)` と、`0` から始まる追加順の番号を表す。

### `InsertionResult`

```cpp
struct InsertionResult {
    size_t id;
    bool active;
    size_t deactivated_count;
    size_t maximum_count;
};
```

`active` は追加した区間が、包含される外側区間を除いた極小集合に残ったかを表す。
`deactivated_count` は今回の追加によって極小集合から除かれた外側区間数、`maximum_count` は追加後の最大選択数である。
極小集合は最適解そのものではないため、`active == true` でも `optimal_schedule_ids()` の返す一つの最適解に含まれない場合がある。

## API・時間計算量

座標幅を $U=\mathtt{maximum\_coordinate}-\mathtt{minimum\_coordinate}$、追加済み区間数を $N$、現在の極小区間数を $A$ とする。

- `IncrementalIntervalScheduling(minimum_coordinate, maximum_coordinate)`: 使用する整数座標範囲 `[minimum_coordinate, maximum_coordinate]` を固定して空の構造を作る。時間・領域 $O(U)$。
- `minimum_coordinate()`: 構築時の最小座標を返す。時間・追加領域 $O(1)$。
- `maximum_coordinate()`: 構築時の最大座標を返す。時間・追加領域 $O(1)$。
- `reserve_intervals(count)`: 区間本体とactiveフラグの領域を少なくとも `count` 件分予約する。時間 $O(N)$、追加領域 $O(\mathtt{count})$。
- `insert(start, finish)`: 任意の半開区間 `[start, finish)` を追加する。今回除去される外側区間数を $D$ として、時間 $O((D+1)\log U+(D+1)\log(A+1))$、Link-Cut Tree以外の追加領域 $O(1)$ amortized。各区間は高々一度しか除去されないため、追加列全体では1回あたり $O(\log(U+N))$ amortized。
- `interval_count()`: 追加済み区間数を返す。時間・追加領域 $O(1)$。
- `active_interval_count()`: 現在の極小区間数を返す。時間・追加領域 $O(1)$。
- `maximum_count()`: 現在選択できる最大区間数を返す。時間・追加領域 $O(1)$。
- `is_active(id)`: 区間 `id` が現在の極小集合に残っているかを返す。時間・追加領域 $O(1)$。
- `interval(id)`: 区間 `id` へのconst参照を返す。時間・追加領域 $O(1)$。
- `optimal_schedule_ids()`: 最大個数を達成する一つの区間番号列を時間順に返す。返却数を $K$ として時間 $O(K\log(A+1))$、追加領域 $O(K)$。

全体の保存領域は $O(U+N)$。

## 注意点

- `minimum_coordinate < maximum_coordinate` が必要であり、違反時は `invalid_argument`。
- `start < finish` かつ `minimum_coordinate <= start < finish <= maximum_coordinate` が必要である。前者の違反時は `invalid_argument`、範囲外では `out_of_range`。
- 座標幅は `int` の正の範囲に収まる必要があり、超過時は `length_error`。座標幅に比例する初期化とメモリを必要とする。
- 任意の包含・交差・重複を許す。重複区間は最大個数を増やさず、極小集合には代表1件だけが残る。
- 端点が接する `[a,b)` と `[b,c)` は重ならない。
- 区間の明示的な削除や端点変更は提供しない。
- `active` は内部の極小区間集合への所属であり、特定の最適スケジュールへの所属ではない。
- 範囲外の `id` を `is_active` または `interval` に渡すと `out_of_range`。
