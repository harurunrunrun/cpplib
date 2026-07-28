---
title: LZ77 Factorization (LZ77分解)
documentation_of: ../../../../src/algorithm/string/compression/lz77_factorization.hpp
---

各位置で、それ以前に開始する接尾辞との最長一致と、その直後の1 byte をfactorにする。
接尾辞順位の処理済み集合では、最長一致候補は直前・直後の2接尾辞だけでよい。

## API

```cpp
std::vector<LZ77Factor> lz77_factorization(const std::string& text)
std::string decode_lz77(const std::vector<LZ77Factor>& factors)
```

`LZ77Factor` は `source`, `length`, `next_symbol`, `has_next_symbol` を持つ。
一致は自己参照による重なりを許す。末尾まで一致したfactorだけは次の記号を持たない。

## API別の時間計算量・空間計算量

$N$ を元文字列長、$Z$ を復号長とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `lz77_factorization` | $O(N\log N)$ | $O(N\log N)$ |
| `decode_lz77` | $O(Z)$ | $O(Z)$ |

## 注意点

`decode_lz77` には同じAPIが生成した、参照範囲が有効なfactor列を渡す。
