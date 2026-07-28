---
title: Rake–Compress Tree Contraction (レイク・コンプレス木縮約)
documentation_of: ../../../../src/algorithm/tree/decomposition/rake_compress_tree_contraction.hpp
---

葉の除去（rake）と次数2頂点の縮約（compress）を独立なラウンドとして列挙する。

## API

- `TreeContractionStep`: 操作種別、除去頂点、接続先を保持する。
- `rake_compress_tree_contraction(graph)`: 縮約ラウンド列を返す。

## 時間計算量

- $O(N\log N)$ 時間、$O(N\log N)$ 空間。

## 注意点

- 入力は単純な無向木とする。不正なグラフでは例外を送出する。
