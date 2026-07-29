---
title: Product Quantization and IVFADC (直積量子化とIVFADC)
documentation_of: ../../../src/approximate/nearest_neighbor/product_quantization.hpp
---

## Product Quantization / 直積量子化

`ProductQuantizer<Real>` は実行時次元の `std::vector<Real>` を等長部分ベクトルへ分け、各部分空間を独立なLloyd k-meansで量子化する。`Real` は浮動小数点型でなければならない。codeは各部分空間のcentroid添字を並べた `std::vector<std::size_t>` である。

### 構築と符号化

`ProductQuantizer(points, subquantizer_count, centroid_count, iterations, random)` と `reset(...)` がcodebookを学習し、全登録点を符号化する。点集合は空でなく、次元は正かつsubquantizer数で割り切れ、`1 <= centroid_count <= N`、反復回数は正でなければならない。

点数 `N`、次元 `D`、部分空間数 `M`、各codebookのcentroid数 `C`、反復数 `I` に対し、時間計算量は `O(INCD)`、空間計算量は `O(ND+NM+CD)`。`encode(point)` は時間 `O(CD)`、追加空間 `O(M)`、`decode(code)` は時間・出力空間 `O(D)`。

### 距離と検索

`asymmetric_distance(query, code)` はqueryを量子化せず、復号centroidとの二乗距離を `long double` で返す。時間 `O(D)`、追加空間 `O(1)`。

`nearest(query, k)` は全codeをADCで順位付けして `AnnNeighbor` を返す。時間計算量は `O(ND+N log N)`、追加空間計算量は `O(N)`。返す距離は量子化距離であり、元ベクトルへの真の距離ではない。

`size()`, `empty()`, `dimension()`, `subquantizer_count()`, `centroid_count()` は `O(1)`。`code(index)` と `codebook(part)` は内部表現を定数時間で参照する。

## IVFADC

`IvfAdcIndex<Real>` は全次元のcoarse k-meansでinverted listを作り、各点のresidualをProduct Quantizationする。

`IvfAdcIndex(points, coarse_centroid_count, subquantizer_count, pq_centroid_count, iterations, random)` と `reset(...)` で構築する。学習時間計算量は `O(IN(Cc+Cp)D)`、空間計算量は `O(ND+NM+(Cc+Cp)D)`。`Cc` はcoarse centroid数、`Cp` はPQ centroid数である。

`nearest(query, k, probe_count)` はqueryに近い `probe_count` 個のinverted listだけを走査し、residual ADCで順位付けする。走査候補数を `S` として時間 `O(CcD+Cc log Cc+SD+S log S)`、追加空間 `O(D+Cc+S)`。候補が `k` 未満の場合だけ全点の真の距離へfallbackする。`probe_count` を増やすと通常recallが上がるが近似保証はない。

`point`, `coarse_assignment`, `inverted_list`, `quantizer` で構築結果を参照できる。

## 注意点

乱数器は参照で渡し、同じ実装・入力・初期状態なら決定的である。非有限座標、次元不一致、範囲外code、`long double` または `Real` で表現できない距離・residualを拒否する。学習器は空clusterのcentroidを直前値のまま保持する。
