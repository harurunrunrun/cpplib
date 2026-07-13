---
title: MinHash (最小ハッシュ)
documentation_of: ../src/approximate/streaming/minhash.hpp
---

集合の Jaccard 類似度を固定長 signature で推定する。

- `MinHash(seed)`: 全成分を未使用値にする。$O(SignatureSize)$。
- `add(key)`: 各 hash 成分の最小値を更新する。$O(SignatureSize)$。
- `similarity(other)`: 一致成分の割合。$O(SignatureSize)$。
- `merge(other)`: 成分ごとの最小値を取り、集合の和を表す。$O(SignatureSize)$。
- `clear()`: 空集合に戻す。$O(SignatureSize)$。
- `signature()`: signature への const 参照。$O(1)$。
- `seed()`: hash seed を返す。$O(1)$。
- `empty()`, `signature_size()`: $O(1)$。

異なる seed の比較・merge は `std::invalid_argument`。独立 permutation の理論モデルでは期待値は Jaccard 係数 $J$、
標準偏差は $\sqrt{J(1-J)/SignatureSize}$ で、Hoeffding bound は
$\Pr[|\hat J-J|\ge\varepsilon]\le2e^{-2 SignatureSize\varepsilon^2}$。
保存領域は $O(SignatureSize)$。
