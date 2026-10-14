#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_STAR_AUGMENTED_MINIMUM_CUT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_STAR_AUGMENTED_MINIMUM_CUT_HPP_INCLUDED

// Extreme-set contraction adapted from the upstream project:
// graph/global_minimum_cut_of_dynamic_star_augmented_graph/sol/correct.cpp,
// revision 04c8de378bab67be926325de2871f0babb8e6451.
// Copyright upstream contributors. SPDX-License-Identifier: Apache-2.0.
// Full license text appears at the end of this header.
// Changes: reusable class,
// iterative hierarchy/HLD, LCA cut preprocessing, cut-side certificates.

#include <algorithm>
#include <array>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct StarAugmentedCutEdge{
    int from;
    int to;
    long long capacity;
};

class StarAugmentedMinimumCut{
    int vertex_count_;
    int tree_size_;
    std::vector<std::array<int, 2>> children_;
    std::vector<int> parent_, depth_, head_, position_, inverse_;
    std::vector<long long> star_, minimum_, lazy_;
    std::vector<int> argmin_;
    __int128 total_capacity_ = 0;

    void build_segment(int node, int left, int right, const std::vector<long long>& values){
        if(right - left == 1){
            minimum_[node] = values[left];
            argmin_[node] = left;
            return;
        }
        const int middle = (left + right) / 2;
        build_segment(node * 2, left, middle, values);
        build_segment(node * 2 + 1, middle, right, values);
        pull(node);
    }

    void pull(int node){
        const int best = minimum_[node * 2] <= minimum_[node * 2 + 1] ? node * 2 : node * 2 + 1;
        minimum_[node] = minimum_[best];
        argmin_[node] = argmin_[best];
    }

    void apply(int node, long long delta){
        minimum_[node] += delta;
        lazy_[node] += delta;
    }

    void add_range(int node, int left, int right, int begin, int end, long long delta){
        if(end <= left || right <= begin) return;
        if(begin <= left && right <= end){
            apply(node, delta);
            return;
        }
        if(lazy_[node]){
            apply(node * 2, lazy_[node]);
            apply(node * 2 + 1, lazy_[node]);
            lazy_[node] = 0;
        }
        const int middle = (left + right) / 2;
        add_range(node * 2, left, middle, begin, end, delta);
        add_range(node * 2 + 1, middle, right, begin, end, delta);
        pull(node);
    }

    int lca(int first, int second) const{
        while(head_[first] != head_[second]){
            if(depth_[head_[first]] < depth_[head_[second]]) std::swap(first, second);
            first = parent_[head_[first]];
        }
        return depth_[first] < depth_[second] ? first : second;
    }

public:
    StarAugmentedMinimumCut(
        int vertex_count,
        const std::vector<StarAugmentedCutEdge>& edges,
        std::vector<long long> star_weights = {}
    ) : vertex_count_(vertex_count), tree_size_(2 * vertex_count - 1){
        if(vertex_count <= 0) throw std::invalid_argument("StarAugmentedMinimumCut: empty graph");
        if(star_weights.empty()) star_weights.resize(static_cast<std::size_t>(vertex_count));
        if(static_cast<int>(star_weights.size()) != vertex_count){
            throw std::invalid_argument("StarAugmentedMinimumCut: invalid star size");
        }
        star_ = std::move(star_weights);
        for(long long weight: star_){
            if(weight < 0) throw std::invalid_argument("StarAugmentedMinimumCut: negative capacity");
            total_capacity_ += weight;
        }
        using Adjacent = std::pair<int, long long>;
        std::vector<std::vector<Adjacent>> graph(static_cast<std::size_t>(tree_size_));
        for(const auto& edge: edges){
            if(edge.from < 0 || edge.from >= vertex_count || edge.to < 0
                || edge.to >= vertex_count || edge.capacity < 0){
                throw std::invalid_argument("StarAugmentedMinimumCut: invalid edge");
            }
            if(edge.from == edge.to || edge.capacity == 0) continue;
            total_capacity_ += edge.capacity;
            graph[edge.from].push_back({edge.to, edge.capacity});
            graph[edge.to].push_back({edge.from, edge.capacity});
        }
        if(total_capacity_ > std::numeric_limits<long long>::max()){
            throw std::overflow_error("StarAugmentedMinimumCut: capacity sum overflow");
        }
        children_.assign(static_cast<std::size_t>(tree_size_), {-1, -1});
        parent_.assign(static_cast<std::size_t>(tree_size_), -1);
        std::vector<unsigned char> active(static_cast<std::size_t>(tree_size_));
        std::fill(active.begin(), active.begin() + vertex_count, 1);
        for(int phase = 0; phase + 1 < vertex_count; ++phase){
            const int merged = vertex_count + phase;
            std::vector<long long> degree(static_cast<std::size_t>(merged));
            std::vector<unsigned char> used(static_cast<std::size_t>(merged));
            using Entry = std::pair<long long, int>;
            std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
            for(int vertex = 0; vertex < merged; ++vertex){
                if(!active[vertex]) continue;
                for(const auto& adjacent: graph[vertex]) degree[vertex] += adjacent.second;
                queue.push({degree[vertex], vertex});
            }
            int previous = -1, last = -1;
            for(int step = 0; step < vertex_count - phase; ++step){
                while(used[queue.top().second] || queue.top().first != degree[queue.top().second]) queue.pop();
                const int vertex = queue.top().second;
                queue.pop();
                used[vertex] = 1;
                previous = last;
                last = vertex;
                for(const auto& [next, capacity]: graph[vertex]){
                    if(used[next]) continue;
                    degree[next] -= capacity;
                    queue.push({degree[next], next});
                }
            }
            children_[merged] = {previous, last};
            parent_[previous] = parent_[last] = merged;
            active[previous] = active[last] = 0;
            graph[previous].clear();
            graph[last].clear();
            for(int vertex = 0; vertex < merged; ++vertex){
                if(!active[vertex]) continue;
                for(auto& [next, capacity]: graph[vertex]){
                    if(next == previous || next == last){
                        next = merged;
                        graph[merged].push_back({vertex, capacity});
                    }
                }
            }
            active[merged] = 1;
        }

        std::vector<int> size(static_cast<std::size_t>(tree_size_), 1);
        std::vector<int> heavy(static_cast<std::size_t>(tree_size_), -1);
        for(int vertex = vertex_count; vertex < tree_size_; ++vertex){
            const auto [first, second] = children_[vertex];
            size[vertex] += size[first] + size[second];
            heavy[vertex] = size[first] >= size[second] ? first : second;
        }
        depth_.resize(static_cast<std::size_t>(tree_size_));
        for(int vertex = tree_size_ - 1; vertex >= vertex_count; --vertex){
            for(int child: children_[vertex]) depth_[child] = depth_[vertex] + 1;
        }
        head_.resize(static_cast<std::size_t>(tree_size_));
        position_.resize(static_cast<std::size_t>(tree_size_));
        inverse_.resize(static_cast<std::size_t>(tree_size_));
        std::vector<int> pending{tree_size_ - 1};
        int next_position = 0;
        while(!pending.empty()){
            const int top = pending.back();
            pending.pop_back();
            for(int vertex = top; vertex != -1; vertex = heavy[vertex]){
                head_[vertex] = top;
                position_[vertex] = next_position;
                inverse_[next_position++] = vertex;
                if(vertex >= vertex_count){
                    for(int child: children_[vertex]) if(child != heavy[vertex]) pending.push_back(child);
                }
            }
        }
        std::vector<__int128> boundary(static_cast<std::size_t>(tree_size_));
        for(const auto& edge: edges){
            if(edge.from == edge.to || edge.capacity == 0) continue;
            boundary[edge.from] += edge.capacity;
            boundary[edge.to] += edge.capacity;
            boundary[lca(edge.from, edge.to)] -= static_cast<__int128>(2) * edge.capacity;
        }
        for(int vertex = 0; vertex < vertex_count; ++vertex) boundary[vertex] += star_[vertex];
        for(int vertex = 0; vertex + 1 < tree_size_; ++vertex) boundary[parent_[vertex]] += boundary[vertex];
        std::vector<long long> initial(static_cast<std::size_t>(tree_size_));
        for(int vertex = 0; vertex < tree_size_; ++vertex){
            initial[position_[vertex]] = static_cast<long long>(boundary[vertex]);
        }
        minimum_.resize(static_cast<std::size_t>(4 * tree_size_));
        lazy_.resize(static_cast<std::size_t>(4 * tree_size_));
        argmin_.resize(static_cast<std::size_t>(4 * tree_size_));
        build_segment(1, 0, tree_size_, initial);
    }

    long long minimum_cut() const{
        return minimum_[1];
    }

    long long set_weight(int vertex, long long capacity){
        if(vertex < 0 || vertex >= vertex_count_ || capacity < 0){
            throw std::invalid_argument("StarAugmentedMinimumCut: invalid update");
        }
        const long long delta = capacity - star_[vertex];
        if(total_capacity_ + delta > std::numeric_limits<long long>::max()){
            throw std::overflow_error("StarAugmentedMinimumCut: capacity sum overflow");
        }
        total_capacity_ += delta;
        star_[vertex] = capacity;
        while(vertex != -1){
            add_range(1, 0, tree_size_, position_[head_[vertex]], position_[vertex] + 1, delta);
            vertex = parent_[head_[vertex]];
        }
        return minimum_cut();
    }

    std::vector<int> minimum_cut_side() const{
        std::vector<int> result;
        std::vector<int> pending{inverse_[argmin_[1]]};
        while(!pending.empty()){
            const int vertex = pending.back();
            pending.pop_back();
            if(vertex < vertex_count_) result.push_back(vertex);
            else for(int child: children_[vertex]) pending.push_back(child);
        }
        return result;
    }
};


/*
Third-party license for the adapted portions of this header.

                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      "License" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      "Licensor" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      "Legal Entity" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      "control" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity.

      "You" (or "Your") shall mean an individual or Legal Entity
      exercising permissions granted by this License.

      "Source" form shall mean the preferred form for making modifications,
      including but not limited to software source code, documentation
      source, and configuration files.

      "Object" form shall mean any form resulting from mechanical
      transformation or translation of a Source form, including but
      not limited to compiled object code, generated documentation,
      and conversions to other media types.

      "Work" shall mean the work of authorship, whether in Source or
      Object form, made available under the License, as indicated by a
      copyright notice that is included in or attached to the work
      (an example is provided in the Appendix below).

      "Derivative Works" shall mean any work, whether in Source or Object
      form, that is based on (or derived from) the Work and for which the
      editorial revisions, annotations, elaborations, or other modifications
      represent, as a whole, an original work of authorship. For the purposes
      of this License, Derivative Works shall not include works that remain
      separable from, or merely link (or bind by name) to the interfaces of,
      the Work and Derivative Works thereof.

      "Contribution" shall mean any work of authorship, including
      the original version of the Work and any modifications or additions
      to that Work or Derivative Works thereof, that is intentionally
      submitted to Licensor for inclusion in the Work by the copyright owner
      or by an individual or Legal Entity authorized to submit on behalf of
      the copyright owner. For the purposes of this definition, "submitted"
      means any form of electronic, verbal, or written communication sent
      to the Licensor or its representatives, including but not limited to
      communication on electronic mailing lists, source code control systems,
      and issue tracking systems that are managed by, or on behalf of, the
      Licensor for the purpose of discussing and improving the Work, but
      excluding communication that is conspicuously marked or otherwise
      designated in writing by the copyright owner as "Not a Contribution."

      "Contributor" shall mean Licensor and any individual or Legal Entity
      on behalf of whom a Contribution has been received by Licensor and
      subsequently incorporated within the Work.

   2. Grant of Copyright License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      copyright license to reproduce, prepare Derivative Works of,
      publicly display, publicly perform, sublicense, and distribute the
      Work and such Derivative Works in Source or Object form.

   3. Grant of Patent License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      (except as stated in this section) patent license to make, have made,
      use, offer to sell, sell, import, and otherwise transfer the Work,
      where such license applies only to those patent claims licensable
      by such Contributor that are necessarily infringed by their
      Contribution(s) alone or by combination of their Contribution(s)
      with the Work to which such Contribution(s) was submitted. If You
      institute patent litigation against any entity (including a
      cross-claim or counterclaim in a lawsuit) alleging that the Work
      or a Contribution incorporated within the Work constitutes direct
      or contributory patent infringement, then any patent licenses
      granted to You under this License for that Work shall terminate
      as of the date such litigation is filed.

   4. Redistribution. You may reproduce and distribute copies of the
      Work or Derivative Works thereof in any medium, with or without
      modifications, and in Source or Object form, provided that You
      meet the following conditions:

      (a) You must give any other recipients of the Work or
          Derivative Works a copy of this License; and

      (b) You must cause any modified files to carry prominent notices
          stating that You changed the files; and

      (c) You must retain, in the Source form of any Derivative Works
          that You distribute, all copyright, patent, trademark, and
          attribution notices from the Source form of the Work,
          excluding those notices that do not pertain to any part of
          the Derivative Works; and

      (d) If the Work includes a "NOTICE" text file as part of its
          distribution, then any Derivative Works that You distribute must
          include a readable copy of the attribution notices contained
          within such NOTICE file, excluding those notices that do not
          pertain to any part of the Derivative Works, in at least one
          of the following places: within a NOTICE text file distributed
          as part of the Derivative Works; within the Source form or
          documentation, if provided along with the Derivative Works; or,
          within a display generated by the Derivative Works, if and
          wherever such third-party notices normally appear. The contents
          of the NOTICE file are for informational purposes only and
          do not modify the License. You may add Your own attribution
          notices within Derivative Works that You distribute, alongside
          or as an addendum to the NOTICE text from the Work, provided
          that such additional attribution notices cannot be construed
          as modifying the License.

      You may add Your own copyright statement to Your modifications and
      may provide additional or different license terms and conditions
      for use, reproduction, or distribution of Your modifications, or
      for any such Derivative Works as a whole, provided Your use,
      reproduction, and distribution of the Work otherwise complies with
      the conditions stated in this License.

   5. Submission of Contributions. Unless You explicitly state otherwise,
      any Contribution intentionally submitted for inclusion in the Work
      by You to the Licensor shall be under the terms and conditions of
      this License, without any additional terms or conditions.
      Notwithstanding the above, nothing herein shall supersede or modify
      the terms of any separate license agreement you may have executed
      with Licensor regarding such Contributions.

   6. Trademarks. This License does not grant permission to use the trade
      names, trademarks, service marks, or product names of the Licensor,
      except as required for reasonable and customary use in describing the
      origin of the Work and reproducing the content of the NOTICE file.

   7. Disclaimer of Warranty. Unless required by applicable law or
      agreed to in writing, Licensor provides the Work (and each
      Contributor provides its Contributions) on an "AS IS" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
      implied, including, without limitation, any warranties or conditions
      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
      PARTICULAR PURPOSE. You are solely responsible for determining the
      appropriateness of using or redistributing the Work and assume any
      risks associated with Your exercise of permissions under this License.

   8. Limitation of Liability. In no event and under no legal theory,
      whether in tort (including negligence), contract, or otherwise,
      unless required by applicable law (such as deliberate and grossly
      negligent acts) or agreed to in writing, shall any Contributor be
      liable to You for damages, including any direct, indirect, special,
      incidental, or consequential damages of any character arising as a
      result of this License or out of the use or inability to use the
      Work (including but not limited to damages for loss of goodwill,
      work stoppage, computer failure or malfunction, or any and all
      other commercial damages or losses), even if such Contributor
      has been advised of the possibility of such damages.

   9. Accepting Warranty or Additional Liability. While redistributing
      the Work or Derivative Works thereof, You may choose to offer,
      and charge a fee for, acceptance of support, warranty, indemnity,
      or other liability obligations and/or rights consistent with this
      License. However, in accepting such obligations, You may act only
      on Your own behalf and on Your sole responsibility, not on behalf
      of any other Contributor, and only if You agree to indemnify,
      defend, and hold each Contributor harmless for any liability
      incurred by, or claims asserted against, such Contributor by reason
      of your accepting any such warranty or additional liability.

   END OF TERMS AND CONDITIONS

   APPENDIX: How to apply the Apache License to your work.

      To apply the Apache License to your work, attach the following
      boilerplate notice, with the fields enclosed by brackets "[]"
      replaced with your own identifying information. (Don't include
      the brackets!)  The text should be enclosed in the appropriate
      comment syntax for the file format. We also recommend that a
      file or class name and description of purpose be included on the
      same "printed page" as the copyright notice for easier
      identification within third-party archives.

   Copyright 2024 Kohei Morita

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_STAR_AUGMENTED_MINIMUM_CUT_HPP_INCLUDED
