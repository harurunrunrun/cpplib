---
title: Static or Vector Storage (固定配列と可変長配列の共通ストレージ)
documentation_of: ../../../src/structure/array/static_or_vector.hpp
---

`cpplib_detail::StaticOrVector<T, Capacity>` は内部実装用の配列。
`Capacity>=0` は `std::array<T,Capacity>`、`Capacity=-1` は `std::vector<T>` を使う。
`resize(n)` / `reserve(n)` は固定版では容量検証のみ、可変長版では対応するvector操作を行う。
`ensure(n)` は要素数が不足するときだけ `resize(n)` を行い、`fill(x)` は現在の全要素を埋める。
`begin/end/data/size/capacity/operator[]` などの通常の配列操作も提供する。

## 時間計算量

添字アクセス、サイズ取得、固定版の容量検証は $O(1)$。
可変長版のresizeとreserveは標準vectorと同じで、再確保時は既存要素数と追加要素数の和に比例する。
fillは現在の要素数 $N$ に対し $O(N)$、空間は固定版 $O(Capacity)$、可変長版 $O(N)$。

## 注意点

添字アクセスでは自動伸長しない。呼び出し側が読み書きより前に必要な要素数を確保する。
可変長版の再確保は既存の参照・ポインタ・iteratorを無効化する。
`T=bool` の可変長版はvectorのproxy参照を持ち、`data()` を利用できない。
アドレスを必要とするフラグには `unsigned char` などを使う。
