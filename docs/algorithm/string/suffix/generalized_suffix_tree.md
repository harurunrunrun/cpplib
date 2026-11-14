---
title: Generalized Suffix Tree (一般化接尾辞木)
documentation_of: ../../../../src/algorithm/string/suffix/generalized_suffix_tree.hpp
---

複数のバイト文字列を相異なる区切り記号で連結して 1 つの接尾辞索引を構築し、パターンが出現する文字列の番号と位置をまとめて検索する。
内部表現は、接尾辞配列と LCP 配列から線形時間で構築した圧縮木である。

## API

```cpp
explicit GeneralizedSuffixTree(std::vector<std::string> texts)
int text_count() const
std::vector<GeneralizedSuffixTreeOccurrence> locate(
    std::string_view pattern
) const
int count(std::string_view pattern) const
bool contains(std::string_view pattern) const
```

出現を表す型 `GeneralizedSuffixTreeOccurrence` は `int text`（文字列の番号）と `int position`（その文字列内の開始位置）を持ち、`locate` の戻り値はこの組の昇順に並ぶ。
区切りをまたぐ一致は返さない。
空パターンは各文字列の全境界（長さ $L$ の文字列なら $L+1$ 箇所）に一致する。
`contains("")` は文字列が 1 つ以上あれば `true` を返す。

## API 別の時間計算量・空間計算量

全入力長を $N$、パターン長を $M$、報告件数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N)$ |
| `text_count` | $O(1)$ | $O(1)$ |
| `locate` | $O(M\log 256+K\log K)$ | $O(K)$ |
| `count`, `contains` | $O(M\log 256)$ | $O(1)$ |

コンストラクタの $O(N\log N)$ は `std::vector<int>` に対する `suffix_array`（座標圧縮のソートと、入力に応じて選ぶ SA-IS またはダブリング）によるもので、木の構築自体は $O(N)$ である。

## 注意点

入力文字列は構築時にコピーされ、後から追加・変更できない。
