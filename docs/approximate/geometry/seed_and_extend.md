---
title: Seed-and-Extend Hamming Search (Seed-and-Extend Hamming探索)
documentation_of: ../../../src/approximate/geometry/seed_and_extend.hpp
---

`seed_and_extend` は、テキスト中でパターンの部分列（シード）と完全一致する箇所をローリングハッシュで列挙し、そのシードから得られる位置合わせ（アラインメント）だけを、パターン全体のハミング距離で検証する。ハッシュが衝突した場合はシード本体を比較するため、衝突による誤答はない。ギャップを許さない（ungapped）検索である。

## API

```cpp
seed_and_extend(
    std::span<const Symbol> text,
    std::span<const Symbol> pattern,
    std::size_t seed_length,
    std::size_t maximum_mismatches
)
```

`std::vector<Symbol>` を受け取るオーバーロードもある。`Symbol` は `std::hash<Symbol>` と等値比較を提供しなければならない。戻り値は `SeedExtendMatch` の列で、各要素はテキスト上の位置 `text_position` と実際の不一致数 `mismatches` を持ち、`text_position` の昇順に並ぶ。

パターンが空でなく、`1 <= seed_length <= pattern.size()` でなければならない。パターンがテキストより長ければ空の列を返す。

## 完全性

パターン長を $M$、許容する不一致数を $d$ とする。`seed_length` が $\lfloor (M-d)/(d+1) \rfloor$ 以下なら、$d$ 個以下の不一致で分断される一致区間のうち少なくとも 1 つはシード長以上になるので、全ての解をシードから列挙できる。この十分条件を満たさない場合は、見落としを避けるために全てのアラインメントを検証する。したがって戻り値は、パラメータによらず、ハミング距離の条件を満たす位置の集合と厳密に一致する。

## 計算量

テキスト長を $N$、パターン長を $M$、シードの一致の総数を $H$、検証するアラインメントの個数を $A$、`seed_length` を $s$ とする。期待時間計算量は $O(N+M+Hs+AM)$、追加空間計算量は $O(N+H)$ である。完全性の条件を満たす場合、通常 $A$ は全アラインメント数より小さい。ハッシュが集中した場合や、安全側の全検証に切り替えた場合の最悪時間は $O(NM)$ である。

## 注意点

- 置換のみを扱い、挿入・削除や局所アラインメントのスコアは扱わない。編集距離やギャップ付きアラインメントが必要なら、別のアラインメントアルゴリズムを使う。
- 空のパターン、`seed_length == 0`、`seed_length > pattern.size()` では `std::invalid_argument` を送出する。
