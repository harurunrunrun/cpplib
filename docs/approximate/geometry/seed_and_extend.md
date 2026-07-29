---
title: Seed-and-Extend Hamming Search (Seed-and-Extend Hamming探索)
documentation_of: ../../../src/approximate/geometry/seed_and_extend.hpp
---

## Seed-and-Extend Hamming Search / Seed-and-Extend Hamming探索

`seed_and_extend` はtext中の完全一致seedをrolling hashで列挙し、そのseedから得たalignmentだけをpattern全体のHamming距離で検証する。hash衝突時にはseed本体を比較するため、衝突による誤答はない。gapを許さないungapped検索である。

## API

```cpp
seed_and_extend(
    std::span<const Symbol> text,
    std::span<const Symbol> pattern,
    std::size_t seed_length,
    std::size_t maximum_mismatches
)
```

`std::vector<Symbol>` overloadもある。`Symbol` は `std::hash<Symbol>` と等値比較を提供しなければならない。返値 `SeedExtendMatch` は `text_position` と実際の `mismatches` を持ち、text位置の昇順で並ぶ。

patternは空でなく、`1 <= seed_length <= pattern.size()` が必要である。patternがtextより長ければ空列を返す。

## Completeness / 完全性

`seed_length <= (M-d)/(d+1)`（`M` はpattern長、`d` は許容不一致数）なら、`d` 個以下の不一致で分断される一致runの少なくとも一つがseed長以上なので、全解をseedから列挙できる。この十分条件を満たさない場合は、見落としを避けるため全alignmentを検証する。したがって返値はparameterによらずHamming距離条件を満たす位置と厳密に一致する。

## Complexity / 計算量

text長を `N`、pattern長を `M`、seed hit総数を `H`、検証alignment数を `A` とする。期待時間計算量は `O(N+M+H seed_length+AM)`、追加空間計算量は `O(N+H)`。完全性条件を満たす場合は通常 `A` が全alignmentより小さい。hash集中時または安全fallback時の最悪時間は `O(NM)`。

## 注意点

これは置換のみを扱い、挿入・削除やlocal alignment scoreは扱わない。編集距離やgapped alignmentが必要なら別のalignment algorithmを使用する。
