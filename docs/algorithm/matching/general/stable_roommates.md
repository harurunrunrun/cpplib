---
title: Stable Roommates (安定ルームメイト)
documentation_of: ../../../../src/algorithm/matching/general/stable_roommates.hpp
---

偶数人の各人が自分以外の全員に厳密な選好順序を持つとき、組になっていない 2 人が互いに現在の相手より相手を好む、ということのない完全マッチング（安定マッチング）を求める。

## API

```cpp
optional<vector<int>> stable_roommates(
    const vector<vector<int>>& preferences
);
```

安定マッチングが存在すれば、`match[p]` に `p` の相手を格納した列を返し、存在しなければ `nullopt` を返す。空の入力には空のマッチングを返す。

## 時間計算量

Irving 法の第 1 段階とローテーションの除去により、時間 $O(N^2)$、空間 $O(N^2)$。

## 注意点

各選好列は自分以外の全員を重複なくちょうど 1 回ずつ含む必要がある。違反時は `runtime_error` を送出する。人数が奇数の場合は完全マッチングが存在しないため、選好列を検査せずに `nullopt` を返す。
