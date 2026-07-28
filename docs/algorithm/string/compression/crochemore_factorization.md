---
title: Crochemore Factorization (Crochemore分解)
documentation_of: ../../../../src/algorithm/string/compression/crochemore_factorization.hpp
---

各位置から始まる、より前の位置にも出現する最長接頭辞をfactorにする。長さ0なら
新しい1文字をfactorにする。接尾辞配列順位の処理済み集合とLCEを用いる。

## API

```cpp
std::vector<CrochemoreFactor> crochemore_factorization(
    const std::string& text
)
```

`CrochemoreFactor` は `begin`, `length`, `source` を持つ。新しい1文字では
`source == -1` である。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `crochemore_factorization` | $O(N\log N)$ | $O(N\log N)$ |

## 注意点

同じ最長長を実現するsourceが複数あれば最小位置を採用する。
