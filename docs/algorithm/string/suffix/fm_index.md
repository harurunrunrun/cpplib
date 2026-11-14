---
title: FM-Index (FM 索引)
documentation_of: ../../../../src/algorithm/string/suffix/fm_index.hpp
---

末尾に終端記号を付けた文字列の BWT 列について、記号ごとのビットベクトルを保持し、後方検索でパターンに対応する接尾辞配列上の区間を求める。

## API

```cpp
explicit FMIndex(const std::string& text)
int rank(int symbol, int end) const
std::pair<int, int> equal_range(std::string_view pattern) const
int count(std::string_view pattern) const
std::vector<int> locate(std::string_view pattern) const
```

- 内部では終端記号を `0`、バイト `c` を `c + 1` として符号化し、記号の種類は `alphabet_size = 257` である。BWT 列の長さは $N+1$ である。
- `rank(symbol, end)` は BWT 列の `[0, end)` に現れる記号 `symbol`（符号化後の値）の個数を返す。`symbol` が $[0, 257)$ の外、または `end` が $[0, N+1]$ の外の場合は `std::runtime_error` を送出する。
- `equal_range(pattern)` はパターンを接頭辞に持つ接尾辞の、接尾辞配列上の半開区間を返す。
- `count(pattern)` は出現回数を返す。
- `locate(pattern)` は各出現の開始位置を返す。

空パターンは終端位置を含む $N+1$ 箇所に一致する。

## API 別の時間計算量・空間計算量

$N$ を `text` の長さ、$M$ をパターン長、$K$ を一致件数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N+257\lceil N/64\rceil)$ | $O(N+257\lceil N/64\rceil)$ |
| `rank` | $O(1)$ | $O(1)$ |
| `equal_range`, `count` | $O(M)$ | $O(1)$ |
| `locate` | $O(M+K)$ | $O(K)$ |

コンストラクタの $O(N\log N)$ は、`std::vector<int>` に対する `suffix_array`（座標圧縮のソートと SA-IS）による。

## 注意点

`locate` の戻り値は接尾辞配列の順であり、位置の昇順とは限らない。
