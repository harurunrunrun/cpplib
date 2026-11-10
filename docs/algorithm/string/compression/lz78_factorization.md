---
title: LZ78 Factorization (LZ78分解)
documentation_of: ../../../../src/algorithm/string/compression/lz78_factorization.hpp
---

LZ78 分解を求める。既出のフレーズのうち最長のものに次の 1 バイトを加えたものを、新しい辞書フレーズとして順に切り出す。
辞書は各ノードに長さ 256 の遷移表を持つトライで管理する。

## API

```cpp
struct LZ78Factor {
    int prefix = 0;
    unsigned char next_symbol = 0;
    bool has_next_symbol = false;
};

std::vector<LZ78Factor> lz78_factorization(const std::string& text)
std::string decode_lz78(const std::vector<LZ78Factor>& factors)
```

- `prefix` は既出フレーズの辞書番号、`next_symbol` はその後に続く 1 バイトである。
- 入力の末尾が既存のフレーズと一致した場合だけ、最後の因子は次の記号を持たない（`has_next_symbol == false`）。
- `decode_lz78` は因子列から元の文字列を復元する。

## 時間計算量・空間計算量

$N$ を元の文字列の長さ、$F$ をフレーズ数、$Z$ を復号後の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `lz78_factorization` | $O(N+256F)$ | $O(256F)$ |
| `decode_lz78` | $O(Z)$ | $O(Z)$ |

## 注意点

- 辞書番号 0 は空のフレーズである。
- 復号 API には有効な辞書参照を持つ因子列を渡す。範囲は検査しない。
