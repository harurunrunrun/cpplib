---
title: Offline Dynamic Range MEX (更新付きオフライン区間MEX)
documentation_of: ../../../../src/algorithm/range/offline_query/dynamic_range_mex.hpp
---

一点更新と区間 MEX の問い合わせを、更新付き Mo（ブロック幅 $N^{2/3}$）の順序で一括処理する。区間に現れない値の集合は、64 分木に階層化したビット集合で管理する。

## コンストラクタ

```cpp
explicit OfflineDynamicRangeMex(vector<int64_t> initial)
```

初期列を登録する。

## add_update / add_query

```cpp
void add_update(size_t position, int64_t value)
size_t add_query(size_t left, size_t right)
```

操作列を時系列順に追加する。`add_update` は位置 `position` を `value` に変更する操作、`add_query` は半開区間 $[\mathtt{left}, \mathtt{right})$ の MEX の問い合わせを登録し、`add_query` は `solve` の返り値における添字を返す。

## solve

```cpp
vector<size_t> solve() const
```

登録した全クエリの答えを返す。同じオブジェクトで繰り返し呼べる。

## size / update_count / query_count

列長、登録済みの更新数、問い合わせ数を $O(1)$ で返す。

## 時間計算量

$N$ を列長、$Q$ を問い合わせ数、$U$ を更新数、$M=N+Q+U$、値の出入り 1 回の階層数を $L=O(\log_{64}N)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `add_update`, `add_query` | 償却 $O(1)$ | 償却 $O(1)$ |
| `solve` | 典型的に $O(MN^{2/3}L)$ | $O(M+N)$ |
| `size`, `update_count`, `query_count` | $O(1)$ | $O(1)$ |

## 注意点

- 更新・問い合わせは実行順に登録する。これはオフライン構造であり、`add_query` の時点では答えを返さない。
- 範囲外の添字・区間では `out_of_range` を送出する。
