---
title: Hirschberg Algorithm (Hirschberg法)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/sequence_selection/hirschberg_lcs.hpp
---

最長共通部分列を、DP表全体を保持せず分割統治で復元する。

## API

```cpp
template<class Value>
vector<Value> hirschberg_lcs(
    const vector<Value>& first,
    const vector<Value>& second
);
```

最長共通部分列を1つ返す。等価な解が複数ある場合の選択は分割位置に依存する。

## 時間計算量

列長を $N,M$ として $O(NM)$ 時間、再帰と作業列に $O(M+\log N)$ 空間。戻り値を含めると $O(M+L+\log N)$。

## 注意点

`Value` は等値比較とコピーが可能であることを前提とする。第2列を短い方にして呼ぶと補助空間を $O(\min(N,M))$ にできる。
