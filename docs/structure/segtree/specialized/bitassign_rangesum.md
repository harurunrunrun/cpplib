---
title: BitAssign RangeSum (ビット代入・区間和)
documentation_of: ../../../../src/structure/segtree/specialized/bitassign_rangesum.hpp
---

0 と 1 のみからなる列に対して、区間代入と区間和取得を行うデータ構造。64 ビットのブロックと 16 分木を組み合わせ、SIMD とビット演算で定数倍を削っている。

## API

```cpp
Bit01WST()
explicit Bit01WST(size_t n)
explicit Bit01WST(const vector<int>& values)
void init(size_t n)
void build()
size_t size() const
void assign(size_t l, size_t r, bool value)
long long sum(size_t l, size_t r)
bool get(size_t k)
```

- コンストラクタ, `init(n)`: 長さ `n` の全 0 列に初期化する。`values` を渡した場合は非零の要素を 1 とする。
- `build()`: 内部のビット列から集約値を再構築する。
- `assign(l, r, value)`: 半開区間 $[l, r)$ を `value` に代入する。
- `sum(l, r)`: 半開区間 $[l, r)$ の 1 の個数を返す。
- `get(k)`: $k$ 番目のビットを返す。

## 時間計算量

$N$ を列の長さとする。

- `Bit01WST(n)`, `Bit01WST(vector)`, `init`, `build`: $O(N)$
- `assign`, `sum`, `get`: $O(\log N)$
- `size`: $O(1)$

## 注意点

- 添字は 0-indexed、区間は半開区間 $[l, r)$ である。`0 <= l <= r <= size()` と `k < size()` を要求し、違反は `assert` の失敗になる。
- `std::vector` から構築するとき、非零の値は 1 として扱う。
- 保存領域は $O(N/64)$ ワードで、構築以外の各 API の追加の空間計算量は $O(1)$ である。
- x86 の GCC では、ヘッダ内で `#pragma GCC target("avx2,popcnt")` を指定する。AVX2 に対応しない環境では実行できない。
- `bitassign_rangesum.hpp` と `bitassign_rangesum_rangeflip.hpp` はどちらもクラス `Bit01WST` を定義するため、同じ翻訳単位で両方を include することはできない。
