---
title: Dynamic BitOverwrite RangeSum (動的ビット上書き・区間和)
documentation_of: ../../../../src/structure/segtree/dynamic/dynamic_bitoverwrite_rangesum.hpp
---

長さの大きい 0/1 の列に対して、区間を 1 で上書きする更新と区間和の取得を行うデータ構造である。SIMD とビット列による高速化で定数倍を削っている。ノードは必要な部分だけ疎に確保する。

## API

```cpp
template<int B = 16>
struct DynamicWideSetSum;

DynamicWideSetSum()
explicit DynamicWideSetSum(size_t n)
void init(size_t n)
void set(size_t l, size_t r)
uint64_t sum(size_t l, size_t r) const
uint64_t all_sum() const
```

- `B` はノードの分岐数で、`B >= 2` かつ 4 の倍数でなければならない（`static_assert` で検査する）。
- デフォルトコンストラクタは空の列を作る。`DynamicWideSetSum(n)` と `init(n)` は長さ `n` の全要素 0 の列を作る。
- `set(l, r)` は半開区間 `[l, r)` を 1 で上書きする。
- `sum(l, r)` は `[l, r)` にある 1 の個数を、`all_sum()` は全体の 1 の個数を返す。

## 時間計算量

$N$ を列の長さとする。

- デフォルトコンストラクタ、`DynamicWideSetSum(n)`、`init`: $O(1)$
- `set`, `sum`: $O(B\log_B(N/64+1))$
- `all_sum`: $O(1)$

`set` が新しく確保するノード数は $O(\log_B(N/64+1))$ である。保存領域は生成したノード数を $K$ として $O(K)$、クエリの追加空間計算量は $O(\log_B(N/64+1))$ である。

## 注意点

- 添字は 0-indexed、区間は半開区間 `[l, r)` で指定する。
- 範囲違反は `assert` で検査する（`NDEBUG` 下では検査しない）。ノードの確保に失敗すると `std::bad_alloc` を送出する。
- `set` は 0 への代入や反転には対応しない。
