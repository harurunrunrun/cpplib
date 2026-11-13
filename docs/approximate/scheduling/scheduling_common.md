---
title: Scheduling Common Helpers (スケジューリング共通補助)
documentation_of: ../../../src/approximate/scheduling/scheduling_common.hpp
---

スケジューリングのヒューリスティックで、時刻や処理時間を検査する内部 API である。

## API

### `finite_value(value)`, `nonnegative_finite(value)`, `positive_finite(value)`

値が有限か、および符号の条件を満たすかを判定する。時間計算量・追加空間計算量は $O(1)$。

### `as_long_double(value)`

値を有限な `long double` へ変換する。時間計算量・追加空間計算量は $O(1)$。

### `checked_time_add(first, second)`

時刻をオーバーフロー検査付きで加算する。時間計算量・追加空間計算量は $O(1)$。

### `validate_nonnegative_time(value, message)`

非負かつ有限な時刻であることを検査する。時間計算量・追加空間計算量は $O(1)$。

## 注意点

- すべて `approximate::scheduling::scheduling_internal` 名前空間の内部 API である。
- 有限でない値や負の値には `std::invalid_argument` を、変換や加算のオーバーフローには `std::overflow_error` を送出する。
- 独自の数値型の加算オーバーフローは検査できない。
