---
title: Prüfer Code (プリューファーコード)
documentation_of: ../../../../src/algorithm/tree/construction/prufer_code.hpp
---

$N$ 頂点のラベル付き木と、長さ $N-2$ の Prüfer 列を相互に変換する。

## API

```cpp
std::vector<int> prufer_encode(const std::vector<std::vector<int>>& graph)
std::vector<std::pair<int, int>> prufer_decode(const std::vector<int>& code)
```

- `prufer_encode(graph)`: 隣接リストで与えた木を、番号最小の葉から取り除いていく Prüfer 列に符号化する。$N \le 2$ なら空列を返す。
- `prufer_decode(code)`: 頂点番号 $0$ から $\lvert \mathtt{code}\rvert + 1$ までの木の辺の列を復号する。

## 時間計算量

`prufer_encode`, `prufer_decode` ともに $O(N\log N)$ である。

## 注意点

- `graph` は単純な無向木でなければならない。木でないことを検出した場合は `std::invalid_argument` を送出する。
- `prufer_decode` で `code` の要素が $[0, \lvert \mathtt{code}\rvert + 2)$ の範囲外なら `std::out_of_range` を送出する。
