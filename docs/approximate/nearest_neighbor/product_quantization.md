---
title: Product Quantization and IVFADC (直積量子化とIVFADC)
documentation_of: ../../../src/approximate/nearest_neighbor/product_quantization.hpp
---

## Product Quantization

`ProductQuantizer<Real>` は、実行時に次元が決まる `std::vector<Real>` を等長の部分ベクトルに分け、各部分空間を独立な Lloyd 法の $k$-means で量子化する。`Real` は浮動小数点型でなければならない。符号（code）は各部分空間のセントロイドの添字を並べた `std::vector<std::size_t>` である。

### 構築と符号化

`ProductQuantizer(points, subquantizer_count, centroid_count, iterations, random)` と `reset(...)` がコードブックを学習し、全登録点を符号化する。点集合は空でなく、次元は正かつ部分空間の数で割り切れ、`1 <= centroid_count <= N`、反復回数は正でなければならない。

点数を $N$、次元を $D$、部分空間の数を $M$、各コードブックのセントロイド数を $C$、反復回数を $I$ とすると、時間計算量は $O(INCD)$、空間計算量は $O(ND+NM+CD)$ である。`encode(point)` は時間 $O(CD)$、追加空間 $O(M)$、`decode(code)` は時間・出力空間 $O(D)$ である。

### 距離と検索

`asymmetric_distance(query, code)` はクエリを量子化せず、復号したセントロイドとの二乗距離を `long double` で返す（非対称距離計算、ADC）。時間 $O(D)$、追加空間 $O(1)$ である。

`nearest(query, k)` は全符号を ADC で順位付けして `AnnNeighbor` を返す。`1 <= k <= N` でなければならない。時間計算量は $O(ND+N \log N)$、追加空間計算量は $O(N)$ である。返す距離は量子化後の距離であり、元のベクトルへの真の距離ではない。

`size()`, `empty()`, `dimension()`, `subquantizer_count()`, `centroid_count()` は $O(1)$ である。`code(index)` と `codebook(part)` は内部表現を定数時間で参照する。

## IVFADC

`IvfAdcIndex<Real>` は全次元の粗い $k$-means で転置リスト（inverted list）を作り、各点の残差（residual）を直積量子化する。

`IvfAdcIndex(points, coarse_centroid_count, subquantizer_count, pq_centroid_count, iterations, random)` と `reset(...)` で構築する。点集合は空であってはならない。粗いセントロイド数を $C_c$、直積量子化のセントロイド数を $C_p$ とすると、学習の時間計算量は $O(IN(C_c+C_p)D)$、空間計算量は $O(ND+NM+(C_c+C_p)D)$ である。

`nearest(query, k, probe_count)` は、クエリに近い `probe_count` 個の転置リストだけを走査し、残差の ADC で順位付けする。`1 <= k <= N` かつ `1 <= probe_count <= coarse_centroid_count` でなければならない。走査した候補数を $S$ として、時間 $O(C_cD+C_c \log C_c+SD+S \log S)$、追加空間 $O(D+C_c+S)$ である。候補が `k` 個未満の場合に限り、全点の真の距離による探索に切り替える。`probe_count` を増やすと通常は再現率が上がるが、近似の保証はない。

`point`, `coarse_assignment`, `inverted_list`, `quantizer` で構築結果を参照できる。

## 注意点

- 乱数生成器は参照で渡し、同じ実装・入力・初期状態なら決定的である
- 非有限な座標、次元の不一致、不正なパラメータや `k`・`probe_count` には `std::invalid_argument`、範囲外の符号には `std::out_of_range`、`long double` または `Real` で表現できない距離・残差には `std::overflow_error` を送出する
- 学習時、空になったクラスタのセントロイドは直前の値のまま保持する
