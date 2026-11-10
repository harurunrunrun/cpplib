---
title: Crochemore Factorization (Crochemore分解)
documentation_of: ../../../../src/algorithm/string/compression/crochemore_factorization.hpp
---

Crochemore 分解を求める。各位置から始まる接頭辞のうち、より前の位置から始まる部分にも出現する最長のものを因子とする。
その長さが 0 なら、新しい 1 文字を因子とする。
接尾辞配列の順位について処理済み位置の集合（`std::set`）を持ち、順位が隣接する 2 候補との LCE を調べる。

## API

```cpp
struct CrochemoreFactor {
    int begin;
    int length;
    int source;
};

std::vector<CrochemoreFactor> crochemore_factorization(
    const std::string& text
)
```

`begin` は因子の開始位置、`length` は長さ、`source` は同じ部分文字列が始まる、より前の位置である（重なりを許す）。
新しい 1 文字の因子では `length == 1`、`source == -1` である。

## 時間計算量・空間計算量

$N$ を `text` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `crochemore_factorization` | $O(N\log N)$ | $O(N\log N)$ |

## 注意点

同じ最長の長さを実現する `source` が複数ある場合、接尾辞順位で直前・直後の 2 候補のうち位置の小さい方を採用する。
すべての候補の中で最小の位置になるとは限らない。
