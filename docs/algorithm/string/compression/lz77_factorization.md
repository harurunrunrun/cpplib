---
title: LZ77 Factorization (LZ77分解)
documentation_of: ../../../../src/algorithm/string/compression/lz77_factorization.hpp
---

LZ77 分解を求める。各位置について、それより前に開始する接尾辞との最長一致と、その直後の 1 バイトを組にして因子とする。
接尾辞順位で処理済み位置の集合を持てば、最長一致の候補は順位が直前・直後の 2 つの接尾辞だけでよい。

## API

```cpp
struct LZ77Factor {
    int source = -1;
    int length = 0;
    unsigned char next_symbol = 0;
    bool has_next_symbol = false;
};

std::vector<LZ77Factor> lz77_factorization(const std::string& text)
std::string decode_lz77(const std::vector<LZ77Factor>& factors)
```

- `source` は一致の開始位置、`length` は一致長である。一致がなければ `source == -1`、`length == 0` である。
- 一致は自己参照による重なりを許す。
- 末尾まで一致した因子だけは次の記号を持たず、`has_next_symbol == false` となる。
- `decode_lz77` は因子列から元の文字列を復元する。

## 時間計算量・空間計算量

$N$ を元の文字列の長さ、$Z$ を復号後の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `lz77_factorization` | $O(N\log N)$ | $O(N\log N)$ |
| `decode_lz77` | $O(Z)$ | $O(Z)$ |

## 注意点

- `decode_lz77` には、同じ API が生成したもののように参照範囲が有効な因子列を渡す。範囲は検査しない。
- 同じ長さの一致が複数ある場合、接尾辞順位で隣接する 2 候補のうち位置の小さい方を採用する。
