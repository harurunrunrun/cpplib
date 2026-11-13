---
title: Elias-Fano Encoding (Elias-Fano符号)
documentation_of: ../../../src/structure/compressed/elias_fano.hpp
---

単調非減少な符号なし整数列を、上位ビットのビットベクトルと詰めて格納した下位ビットに圧縮する（Elias–Fano 符号）。

## API

~~~cpp
EliasFano()
explicit EliasFano(const std::vector<std::uint64_t>& values)
void build(const std::vector<std::uint64_t>& values)
bool empty() const
std::size_t size() const
std::uint64_t select(std::size_t index) const
std::size_t lower_bound(std::uint64_t value) const
std::size_t rank(std::uint64_t value) const
std::uint64_t universe() const
~~~

- `build(values)`: `values` から作り直す。コンストラクタも同じ処理を行う。
- `select(index)`: 0-indexed で `index` 番目の値を返す。
- `lower_bound(value)`: `value` 以上の最初の要素の添字を返す（なければ `size()`）。
- `rank(value)`: `value` 以下の要素数を返す。
- `universe()`: 列の最大値（末尾の値、空なら 0）を返す。

重複した値もそのまま保持する。

## 時間計算量

要素数を $N$、最大値を $U$ とする。

- 構築: $O(N+U/2^{\ell})$（$\ell$ は下位ビット数で、$U/2^{\ell}=O(N)$ なので $O(N)$）
- `select`: 最悪 $O(1)$
- `lower_bound`, `rank`: 最悪 $O(\log(N+1))$（`select` を使った二分探索）
- `empty`, `size`, `universe`: $O(1)$

## 空間計算量

下位ビットと上位ビットベクトルで $O(N\log(U/N+1)+N)$ ビットを使う。select 用の索引は 1 の位置を 256 個ごとに標本化する。標本間のブロックが 65536 ビット以下なら定数個のワードを走査し、それより長い疎なブロックでは、そのブロック内の 1 の位置をすべて直接保存する。したがって局所的に非常に長い 0 の連続があっても select の時間は増えない。長いブロックは互いに素でそれぞれ $256^2$ を超える上位ビットの位置を占めるので、select 用の索引は全体で $O(N/256)$ ワードに収まる。

## 注意点

- 入力が単調非減少でなければ `std::invalid_argument` を投げる。
- 範囲外の `select` は `std::out_of_range` を投げる。
