---
title: Offline Dynamic Range MEX (更新付きオフライン区間MEX)
documentation_of: ../../../../src/algorithm/range/offline_query/dynamic_range_mex.hpp
---

一点更新と区間 MEX を変更時刻付き Mo 順序で一括処理する。欠損値は階層化した 64 bit 集合で管理する。

## constructor

```cpp
explicit OfflineDynamicRangeMex(vector<int64_t> initial)
```

初期列を登録する。

## add_update / add_query

```cpp
void add_update(size_t position, int64_t value)
size_t add_query(size_t left, size_t right)
```

操作列を時系列順に追加する。`add_query` は答え vector の添字を返す。

## solve

```cpp
vector<size_t> solve() const
```

登録した全クエリの答えを返す。同じ object で繰り返し呼べる。

## size / update_count / query_count

列長と登録操作数を $O(1)$ で返す。

## 時間計算量

$M=N+Q+U$、値の出入り 1 回の階層数を $L=O(\log_{64}N)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `add_update`, `add_query` | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve` | 典型的に $O(MN^{2/3}L)$ | $O(M+N)$ |
| getter | $O(1)$ | $O(1)$ |

## 注意点

更新・クエリは実行順に登録する。不正添字・区間は `out_of_range` を送出する。これはオフライン構造であり、`add_query` 時点では答えを返さない。
