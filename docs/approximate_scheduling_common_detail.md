---
title: Scheduling Common Helpers (schedule共通補助)
documentation_of: ../src/approximate/scheduling/scheduling_common.hpp
---

## Scheduling Common Helpers (schedule共通補助)

schedule heuristicで時刻・処理時間を検査する内部API。

## API

### `finite_value(value)`, `nonnegative_finite(value)`, `positive_finite(value)`

値の有限性と符号を判定する。時間計算量・追加空間計算量は $O(1)$。

### `as_long_double(value)`

値を有限な `long double` へ変換する。時間計算量・追加空間計算量は $O(1)$。

### `checked_time_add(first, second)`

時刻をoverflow検査付きで加算する。時間計算量・追加空間計算量は $O(1)$。

### `validate_nonnegative_time(value, message)`

非負有限時刻であることを検査する。時間計算量・追加空間計算量は $O(1)$。

## 注意点

すべて `approximate::scheduling::scheduling_internal` の内部APIである。非有限値・負値は `std::invalid_argument`、変換または加算overflowは `std::overflow_error` を送出する。独自数値型の加算overflowは検査できない。
