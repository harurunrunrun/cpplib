---
title: BitOverwrite RangeSum (ビット上書き・区間和)
documentation_of: ../../../../src/structure/segtree/specialized/bitoverwrite_rangesum.hpp
---

0 と 1 のみからなる列に対して、区間を 1 で上書きする操作と区間和取得を行うデータ構造。64 ビットのブロックを葉とする $B$ 分木（Wide Segment Tree）で、SIMD とビット演算により定数倍を削っている。

## API

```cpp
template<int B = 16> struct WideSetSum;

WideSetSum()
explicit WideSetSum(size_t n)
void init(size_t n)
void set(size_t l, size_t r)
uint64_t sum(size_t l, size_t r) const
uint64_t all_sum() const
```

- コンストラクタ, `init(n)`: 長さ `n` の全 0 列に初期化する。
- `set(l, r)`: 半開区間 $[l, r)$ を 1 で上書きする。
- `sum(l, r)`, `all_sum()`: 区間・全体の 1 の個数を返す。

## 時間計算量

$N$ を列の長さとする。

- `WideSetSum(n)`, `init`: $O(N/64)$
- `set`, `sum`: $O(B\log_B(N/64+1))$
- `all_sum`: $O(1)$

## 注意点

- 添字は 0-indexed、区間は半開区間 $[l, r)$ である。`0 <= l <= r <= n` を要求し、違反は `assert` の失敗になる（`size()` 関数はなく、長さは公開メンバ `n` に入っている）。
- `set` は 0 への代入や反転には対応しない。
- `B` は 2 以上で、AVX2 を使う場合は 4 の倍数でなければならない（`static_assert`）。
- x86 の GCC では、`WST_NO_AVX2` を定義しない限りヘッダ内で `#pragma GCC target("avx2,popcnt,bmi,bmi2")` を指定する。AVX2 に対応しない環境では `WST_NO_AVX2` を定義してから include する。
- 保存領域は $O(N/64)$ ワードで、構築以外の各 API の追加の空間計算量は $O(1)$ である。
