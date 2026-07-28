---
title: Generalized Suffix Tree (一般化接尾辞木)
documentation_of: ../../../../src/algorithm/string/suffix/generalized_suffix_tree.hpp
---

複数のbyte文字列を相異なる区切り記号で連結して一つの接尾辞索引を構築し、
パターンの出現元文字列と位置をまとめて検索する。
内部表現は接尾辞配列とLCP配列から線形構築した圧縮木である。

## API

```cpp
GeneralizedSuffixTree(std::vector<std::string> texts)
int text_count() const
std::vector<GeneralizedSuffixTreeOccurrence> locate(
    std::string_view pattern
) const
int count(std::string_view pattern) const
bool contains(std::string_view pattern) const
```

出現型は `text` と `position` を持ち、返り値はこの組の昇順である。
区切りをまたぐ一致は返さない。空パターンは各文字列の全境界に一致する。

## API別の時間計算量・空間計算量

全入力長を $N$、パターン長を $M$、報告件数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `text_count` | $O(1)$ | $O(1)$ |
| `locate` | $O(M\log 256+K\log K)$ | $O(K)$ |
| `count`, `contains` | $O(M\log 256)$ | $O(1)$ |

## 注意点

入力文字列は構築時にコピーされ、後から追加・変更できない。
