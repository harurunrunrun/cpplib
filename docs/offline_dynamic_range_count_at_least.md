---
title: Offline Dynamic Range Count At Least (オフライン動的区間下限個数) [GIVEAWAY]
documentation_of: ../src/algorithm/other/offline_dynamic_range_count_at_least.hpp
---

点代入と区間内の下限以上の要素数問い合わせを、操作列が事前に分かる場合に処理する。

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

## Constructor

初期列を保持する。

## size

列長を返す。

## add_set

指定位置を `value` に変更する操作を末尾へ登録する。

## add_count_at_least

半開区間 `[left,right)` 内で `threshold` 以上の要素数を尋ねる操作を末尾へ登録する。

## solve

登録順に操作を実行し、問い合わせ操作の答えだけを順に返す。登録内容と初期列は変更しないため再実行できる。

## API別の時間計算量・空間計算量

列長を $N$、操作数を $Q$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| 各登録API | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve()` | $O((N+Q)\log^2(N+Q))$ | $O((N+Q)\log(N+Q))$ |

## 注意点

- 添字は0-indexed、区間は半開区間で指定する。
- 添字または区間の前提違反時は `runtime_error` を送出する。
- `solve()` 後にも操作を追加でき、次回の `solve()` は全操作を初めから再実行する。
