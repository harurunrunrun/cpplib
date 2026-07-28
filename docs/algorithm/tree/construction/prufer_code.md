---
title: Prüfer Code (プリューファーコード)
documentation_of: ../../../../src/algorithm/tree/construction/prufer_code.hpp
---

ラベル付き木と長さ $N-2$ のPrüfer列を相互変換する。

## API

- `prufer_encode(graph)`: 木を辞書順最小の葉を用いるPrüfer列へ符号化する。
- `prufer_decode(code)`: 頂点番号 `0..|code|+1` の木の辺列を復号する。

## 時間計算量

- `prufer_encode`, `prufer_decode`: $O(N\log N)$。

## 注意点

- `graph` は単純な無向木でなければならない。不正な入力では例外を送出する。
