---
title: Stable Roommates (安定ルームメイト)
documentation_of: ../../../../src/algorithm/matching/general/stable_roommates.hpp
---

偶数人数の各人が他の全員に厳密な選好順序を持つとき、互いに組になっていない2人が現在の相手より互いを好むことのない完全マッチングを求める。

## API

```cpp
optional<vector<int>> stable_roommates(
    const vector<vector<int>>& preferences
);
```

安定マッチングが存在すれば `match[p]` に相手を返し、存在しなければ `nullopt` を返す。空入力には空のマッチングを返す。

## 時間計算量

Irving法の第1段階とrotation除去により $O(N^2)$ 時間、$O(N^2)$ 空間。

## 注意点

各選好列は自分以外の全員を重複なくちょうど1回含む必要がある。違反時は `runtime_error` を送出する。奇数人数では完全マッチングがないため `nullopt` を返す。
