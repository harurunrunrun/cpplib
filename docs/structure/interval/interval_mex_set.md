---
title: Interval Update MEX Set (区間更新MEX集合)
documentation_of: ../../../src/structure/interval/interval_mex_set.hpp
---

巨大な有限の整数領域に対する、区間の追加（set）・削除（clear）・反転（flip）と MEX の取得を、動的に節点を作る遅延セグメント木で処理する。

## コンストラクタ

```cpp
explicit IntervalMexSet(uint64_t universe)
```

$[0, \mathrm{universe})$ のすべての値が欠けている状態を作る。

## assign / flip

```cpp
void assign(uint64_t left, uint64_t right, bool present)
void flip(uint64_t left, uint64_t right)
```

`assign` は半開区間 $[\mathrm{left}, \mathrm{right})$ の値を一括して、`present` が `true` なら存在する状態に、`false` なら欠けている状態にする。`flip` は区間内の存在状態を反転する。

## mex

```cpp
uint64_t mex()
```

欠けている最小の値を返す。領域のすべての値が存在する場合は `universe` を返す。

## universe / distinct_size / node_count

```cpp
uint64_t universe() const
uint64_t distinct_size() const
size_t node_count() const
```

領域の長さ、存在する値の個数、現在確保している節点数を返す。

## 時間計算量

$C = \mathrm{universe}$ とし、更新で生成される節点の総数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(1)$ | $O(1)$ |
| `assign`, `flip` | $O(\log C)$ | 償却 $O(\log C)$ |
| `mex` | $O(\log C)$ | 償却 $O(\log C)$ |
| `universe`, `distinct_size`, `node_count` | $O(1)$ | $O(1)$ |

## 空間計算量

全体で $O(K)$。

## 注意点

- `universe > 0` が必要である。違反するとコンストラクタが `std::invalid_argument` を送出する。
- 区間は $0 \le \mathrm{left} \le \mathrm{right} \le \mathrm{universe}$ を満たす必要がある。違反すると `std::out_of_range` を送出する。
