#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_TDIGEST_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_TDIGEST_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>

template<std::size_t MaxCentroids = 200>
class TDigest {
    static_assert(MaxCentroids >= 2, "MaxCentroids must be at least two");

public:
    struct Centroid {
        long double mean = 0.0L;
        long double weight = 0.0L;
    };

private:
    std::array<Centroid, MaxCentroids + 1U> centroids_{};
    std::size_t size_ = 0;
    long double total_weight_ = 0.0L;

    void reduce_one() noexcept {
        std::size_t best = 0;
        long double best_score = std::numeric_limits<long double>::infinity();
        long double prefix = 0.0L;
        for(std::size_t i = 0; i + 1U < size_; ++i){
            const long double combined = centroids_[i].weight + centroids_[i + 1U].weight;
            const long double quantile = (prefix + combined / 2.0L) / total_weight_;
            const long double tail_scale = std::max(1.0L / total_weight_,
                4.0L * quantile * (1.0L - quantile));
            const long double score =
                (centroids_[i + 1U].mean - centroids_[i].mean) / tail_scale;
            if(score < best_score){
                best_score = score;
                best = i;
            }
            prefix += centroids_[i].weight;
        }
        Centroid& first = centroids_[best];
        const Centroid second = centroids_[best + 1U];
        const long double combined = first.weight + second.weight;
        first.mean = (first.mean * first.weight + second.mean * second.weight) / combined;
        first.weight = combined;
        for(std::size_t i = best + 1U; i + 1U < size_; ++i) centroids_[i] = centroids_[i + 1U];
        --size_;
    }

public:
    void clear() noexcept {
        size_ = 0;
        total_weight_ = 0.0L;
    }

    void add(long double value, long double weight = 1.0L) {
        if(!std::isfinite(value)) throw std::invalid_argument("value must be finite");
        if(!(weight > 0.0L) || !std::isfinite(weight)){
            throw std::invalid_argument("weight must be finite and positive");
        }
        const long double next_total = total_weight_ + weight;
        if(!std::isfinite(next_total)) throw std::overflow_error("total weight overflow");
        std::size_t position = 0;
        while(position < size_ && centroids_[position].mean <= value) ++position;
        for(std::size_t i = size_; i > position; --i) centroids_[i] = centroids_[i - 1U];
        centroids_[position] = Centroid{value, weight};
        ++size_;
        total_weight_ = next_total;
        if(size_ > MaxCentroids) reduce_one();
    }

    void merge(const TDigest& other) {
        const auto snapshot = other.centroids_;
        const std::size_t snapshot_size = other.size_;
        for(std::size_t i = 0; i < snapshot_size; ++i){
            add(snapshot[i].mean, snapshot[i].weight);
        }
    }

    [[nodiscard]] long double quantile(long double probability) const {
        if(size_ == 0) throw std::logic_error("quantile of an empty TDigest");
        if(!(0.0L <= probability && probability <= 1.0L)){
            throw std::out_of_range("probability must be in [0, 1]");
        }
        if(size_ == 1U || probability == 0.0L) return centroids_[0].mean;
        if(probability == 1.0L) return centroids_[size_ - 1U].mean;
        const long double target = probability * total_weight_;
        long double previous_position = centroids_[0].weight / 2.0L;
        if(target <= previous_position) return centroids_[0].mean;
        for(std::size_t i = 1; i < size_; ++i){
            const long double position = previous_position
                + centroids_[i - 1U].weight / 2.0L + centroids_[i].weight / 2.0L;
            if(target <= position){
                const long double ratio = (target - previous_position) / (position - previous_position);
                return centroids_[i - 1U].mean
                    + ratio * (centroids_[i].mean - centroids_[i - 1U].mean);
            }
            previous_position = position;
        }
        return centroids_[size_ - 1U].mean;
    }

    [[nodiscard]] long double cdf(long double value) const {
        if(std::isnan(value)) throw std::invalid_argument("value must not be NaN");
        if(size_ == 0) return 0.0L;
        if(value < centroids_[0].mean) return 0.0L;
        if(value >= centroids_[size_ - 1U].mean) return 1.0L;
        long double cumulative = centroids_[0].weight / 2.0L;
        for(std::size_t i = 1; i < size_; ++i){
            if(value < centroids_[i].mean){
                const long double ratio = (value - centroids_[i - 1U].mean)
                    / (centroids_[i].mean - centroids_[i - 1U].mean);
                const long double interval =
                    (centroids_[i - 1U].weight + centroids_[i].weight) / 2.0L;
                return (cumulative + ratio * interval) / total_weight_;
            }
            cumulative += (centroids_[i - 1U].weight + centroids_[i].weight) / 2.0L;
        }
        return 1.0L;
    }

    [[nodiscard]] std::size_t centroid_count() const noexcept { return size_; }
    [[nodiscard]] long double total_weight() const noexcept { return total_weight_; }
    [[nodiscard]] static constexpr std::size_t maximum_centroids() noexcept {
        return MaxCentroids;
    }
};

#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_TDIGEST_HPP_INCLUDED
