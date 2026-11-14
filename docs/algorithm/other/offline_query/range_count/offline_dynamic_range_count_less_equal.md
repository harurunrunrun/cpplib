---
title: Offline Dynamic Range Count Less Equal (オフライン動的区間以下個数) [RACETIME]
documentation_of: ../../../../../src/algorithm/other/offline_query/range_count/offline_dynamic_range_count_less_equal.hpp
---

一点代入と「区間内で指定値以下の要素の個数」の問い合わせをオフラインで処理する。内部では（位置, 値）を点とする `DynamicPointAddRectangleSum<int, T>` に帰着する。

## API

```cpp
template<class T>
class OfflineDynamicRangeCountLessEqual;
```

- `explicit OfflineDynamicRangeCountLessEqual(vector<T> initial)`: 初期列を保持する。
- `int size() const`: 初期列の要素数を返す。
- `void add_set(int position, T value)`: 位置 `position` に `value` を代入する操作を末尾へ追加する。
- `void add_count_less_equal(int left, int right, T upper)`: 半開区間 $[\mathtt{left}, \mathtt{right})$ で `upper` 以下の要素数を問い合わせる操作を末尾へ追加する。
- `vector<int> solve() const`: 追加順に操作を適用し、問い合わせの答えだけを順に返す。呼び出しても登録内容は変更しない。

## 時間計算量・空間計算量

初期要素数を $N$、登録操作数を $M$、相異なる（位置, 値）の点の数を $P$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `add_set(...)` | 償却 $O(1)$ | 償却 $O(1)$ |
| `add_count_less_equal(...)` | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve()` | $O((P+M)\log^2 P)$ | $O(P\log P+N)$ |

## 注意点

- すべての操作を登録してから `solve()` を呼ぶ。
- `T` は全順序とコピーを持ち、同値比較できなければならない。内部では位置も `T` 型の座標として扱うため、`T` は $0$ 以上 $N$ 未満の `int` の添字を表現できなければならない（`unsigned` などの符号なし型も使える）。
- 添字・区間の違反では `runtime_error` を送出する。
