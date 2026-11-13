---
title: Static or Vector Storage (固定配列と可変長配列の共通ストレージ)
documentation_of: ../../../src/structure/array/static_or_vector.hpp
---

`cpplib_detail::StaticOrVector<T, Capacity>` は内部実装用の配列である。`Capacity >= 0` なら `std::array<T, Capacity>`、`Capacity == -1` なら `std::vector<T>` を内部に持つ。

## API

- `resize(n)`, `resize(n, value)`, `reserve(n)`: 固定長版では容量の検証だけを行い、可変長版では対応する `std::vector` の操作を行う。
- `ensure(n)`: 要素数が `n` に足りないときだけ `resize(n)` を行う。
- `fill(x)`: 現在の全要素を `x` で埋める（固定長版では `Capacity` 個すべて）。
- `size`, `empty`, `capacity`, `operator[]`, `at`, `begin`/`end`, `data`, `front`/`back`, `swap` など、通常の配列操作も提供する。固定長版の `size()` は常に `Capacity` を返す。
- `cpplib_detail::checked_int_size(n)`: `std::size_t` の値を `int` に変換する補助関数。`int` に収まらなければ `std::length_error` を投げる。

## 時間計算量

- 添字アクセス、サイズの取得、固定長版の容量検証: $O(1)$
- 可変長版の `resize`・`reserve`: `std::vector` と同じ。再確保が起きると、既存要素数と追加要素数の和に比例する
- `fill`: 現在の要素数を $N$ として $O(N)$
- 空間計算量: 固定長版は $O(\texttt{Capacity})$、可変長版は $O(N)$

## 注意点

- 添字アクセスでは自動的に伸長しない。呼び出し側が読み書きの前に必要な要素数を確保する。
- 固定長版で容量を超える `resize`/`reserve`/`ensure` は `std::length_error` を投げる。
- 可変長版の再確保は、既存の参照・ポインタ・イテレータを無効にする。
- `T = bool` の可変長版は `std::vector<bool>` のプロキシ参照を持ち、`data()` を利用できない。アドレスが必要なフラグには `unsigned char` などを使う。
