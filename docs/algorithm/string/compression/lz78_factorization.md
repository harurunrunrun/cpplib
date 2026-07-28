---
title: LZ78 Factorization (LZ78分解)
documentation_of: ../../../../src/algorithm/string/compression/lz78_factorization.hpp
---

既出phraseの最長接頭辞に次の1 byteを加え、新しい辞書phraseとして分解する。

## API

```cpp
std::vector<LZ78Factor> lz78_factorization(const std::string& text)
std::string decode_lz78(const std::vector<LZ78Factor>& factors)
```

`LZ78Factor` は `prefix`, `next_symbol`, `has_next_symbol` を持つ。
入力末尾が既存phraseと一致した場合だけ次記号を持たない。

## API別の時間計算量・空間計算量

$N$ を元文字列長、phrase数を $F$、復号長を $Z$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `lz78_factorization` | $O(N+256F)$ | $O(256F)$ |
| `decode_lz78` | $O(Z)$ | $O(Z)$ |

## 注意点

辞書番号0は空phraseである。復号APIには有効な辞書参照を持つfactor列を渡す。
