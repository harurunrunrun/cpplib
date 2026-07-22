---
title: Offline Dynamic Range Count Less Equal (オフライン動的区間以下個数) [RACETIME]
documentation_of: ../src/algorithm/other/offline_query/offline_dynamic_range_count_less_equal.hpp
---

一点代入と、区間内で指定値以下の要素数を数える問い合わせをオフラインで処理します。

## API

### `OfflineDynamicRangeCountLessEqual(initial)`

初期列を保持します。

### `size()`

初期列の要素数を返します。

### `add_set(position, value)`

指定位置を `value` に代入する操作を末尾へ追加します。

### `add_count_less_equal(left, right, upper)`

半開区間 `[left, right)` で `upper` 以下の要素数を返す問い合わせを末尾へ追加します。

### `solve()`

追加順に操作を適用し、個数問い合わせの答えだけを順番に返します。呼び出しても登録内容は変更しません。

## API別の時間計算量・空間計算量

初期要素数を $N$、登録操作数を $M$、異なる更新候補点数を $P$ とします。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `add_set(...)` | 償却 $O(1)$ | 償却 $O(1)$ |
| `add_count_less_equal(...)` | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve()` | $O((P+M)\log^2 P)$ | $O(P\log P+N)$ |

## 注意点

- すべての操作を登録してから `solve()` を呼びます。
- `T` は全順序とコピーを持ち、同値比較できなければなりません。
- 添字・区間違反では `runtime_error` を送出します。
