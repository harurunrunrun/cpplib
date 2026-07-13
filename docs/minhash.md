---
title: MinHash (最小ハッシュ)
documentation_of: ../src/approximate/streaming/minhash.hpp
---

# MinHash (最小ハッシュ)

集合の Jaccard 類似度を固定長 signature で推定する。

## API

- `MinHash(seed)`: 全成分を未使用値にする。時間計算量は $O(SignatureSize)$。 追加空間計算量は $O(1)$。
- `add(key)`: 各 hash 成分の最小値を更新する。時間計算量は $O(SignatureSize)$。 追加空間計算量は $O(1)$。
- `similarity(other)`: 一致成分の割合。時間計算量は $O(SignatureSize)$。 追加空間計算量は $O(1)$。
- `merge(other)`: 成分ごとの最小値を取り、集合の和を表す。時間計算量は $O(SignatureSize)$。 追加空間計算量は $O(1)$。
- `clear()`: 空集合に戻す。時間計算量は $O(SignatureSize)$。 追加空間計算量は $O(1)$。
- `signature()`: signature への const 参照。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `seed()`: hash seed を返す。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `empty()`, `signature_size()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

異なる seed の比較・merge は `std::invalid_argument`。独立 permutation の理論モデルでは期待値は Jaccard 係数 $J$、
標準偏差は $\sqrt{J(1-J)/SignatureSize}$ で、Hoeffding bound は
$\Pr[|\hat J-J|\ge\varepsilon]\le2e^{-2 SignatureSize\varepsilon^2}$。
保存に必要な空間計算量は $O(SignatureSize)$。
