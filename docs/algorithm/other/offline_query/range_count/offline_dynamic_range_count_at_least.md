---
title: Offline Dynamic Range Count At Least (オフライン動的区間下限個数) [GIVEAWAY]
documentation_of: ../../../../../src/algorithm/other/offline_query/range_count/offline_dynamic_range_count_at_least.hpp
---

一点代入と「区間内で下限以上の要素の個数」の問い合わせを、操作列が事前に分かる場合にまとめて処理する。内部では（位置, 値）を点とする `DynamicPointAddRectangleSum` に帰着する。

## OfflineDynamicRangeCountAtLeast

```cpp
class OfflineDynamicRangeCountAtLeast {
public:
    explicit OfflineDynamicRangeCountAtLeast(vector<int> initial);
    int size() const;
    void add_set(int position, int value);
    void add_count_at_least(int left, int right, int threshold);
    vector<int> solve() const;
};
```

## API

- コンストラクタ: 初期列を保持する。
- `size()`: 列長を返す。
- `add_set(position, value)`: 位置 `position` の値を `value` に変更する操作を末尾へ登録する。
- `add_count_at_least(left, right, threshold)`: 半開区間 $[\mathtt{left}, \mathtt{right})$ 内で `threshold` 以上の要素数を問い合わせる操作を末尾へ登録する。
- `solve()`: 登録順に操作を実行し、問い合わせの答えだけを順に返す。登録内容と初期列は変更しないため、再実行できる。

## 時間計算量・空間計算量

列長を $N$、操作数を $Q$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| 各登録 API | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve()` | $O((N+Q)\log^2(N+Q))$ | $O((N+Q)\log(N+Q))$ |

## 注意点

- 添字は 0-indexed、区間は半開区間で指定する。
- `add_set` で添字が範囲外の場合、`add_count_at_least` で $0\le \mathtt{left}\le \mathtt{right}\le N$ を満たさない場合は `runtime_error` を送出する。
- `solve()` の後にも操作を追加でき、次回の `solve()` は全操作を初めから再実行する。
