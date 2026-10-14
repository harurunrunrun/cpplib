#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_MINIMUM_DIAMETER_SPANNING_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_MINIMUM_DIAMETER_SPANNING_TREE_HPP_INCLUDED

// Absolute-center search adapted from the upstream project:
// graph/minimum_diameter_spanning_tree/sol/correct.cpp, revision 04c8de378bab67be926325de2871f0babb8e6451.
// Copyright upstream contributors. SPDX-License-Identifier: Apache-2.0.
// Full license text appears at the end of this header.
// Changes: exact edge IDs,
// signed 64-bit arithmetic, disconnected inputs, half-integral center certificate.

#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct DiameterSpanningTreeEdge{
    int from;
    int to;
    long long weight;
};

struct MinimumDiameterSpanningTreeResult{
    bool exists;
    long long diameter;
    std::vector<int> edge_indices;
    int center_vertex;
    int center_edge;
    long long center_offset_twice;
};

inline MinimumDiameterSpanningTreeResult minimum_diameter_spanning_tree(
    int vertex_count,
    const std::vector<DiameterSpanningTreeEdge>& edges
){
    if(vertex_count <= 0) throw std::invalid_argument("minimum_diameter_spanning_tree: empty graph");
    const int n = vertex_count;
    const long long infinity = std::numeric_limits<long long>::max() / 4;
    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    __int128 total = 0;
    for(int index = 0; index < static_cast<int>(edges.size()); ++index){
        const auto& edge = edges[index];
        if(edge.from < 0 || edge.from >= n || edge.to < 0 || edge.to >= n || edge.weight < 0){
            throw std::invalid_argument("minimum_diameter_spanning_tree: invalid edge");
        }
        if(edge.from == edge.to) continue;
        total += edge.weight;
        if(total >= infinity / 2) throw std::overflow_error("minimum_diameter_spanning_tree: distance overflow");
        graph[edge.from].push_back({edge.to, index});
        graph[edge.to].push_back({edge.from, index});
    }
    const auto shortest_paths = [&](const std::vector<std::pair<int, long long>>& sources,
                                    long long multiplier){
        std::vector<long long> distance(static_cast<std::size_t>(n), infinity);
        std::vector<int> parent_edge(static_cast<std::size_t>(n), -1);
        using Entry = std::pair<long long, int>;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
        for(const auto& [source, initial]: sources){
            distance[source] = initial;
            queue.push({initial, source});
        }
        while(!queue.empty()){
            const auto [length, vertex] = queue.top();
            queue.pop();
            if(length != distance[vertex]) continue;
            for(const auto& [next, index]: graph[vertex]){
                const long long candidate = length + multiplier * edges[index].weight;
                if(candidate < distance[next]){
                    distance[next] = candidate;
                    parent_edge[next] = index;
                    queue.push({candidate, next});
                }
            }
        }
        return std::pair{std::move(distance), std::move(parent_edge)};
    };
    MinimumDiameterSpanningTreeResult result{true, infinity, {}, -1, -1, 0};
    std::vector<std::vector<long long>> distance(static_cast<std::size_t>(n));
    std::vector<std::vector<int>> order(static_cast<std::size_t>(n));
    for(int source = 0; source < n; ++source){
        distance[source] = shortest_paths({{source, 0}}, 1).first;
        if(*std::max_element(distance[source].begin(), distance[source].end()) == infinity){
            result.exists = false;
            result.diameter = 0;
            return result;
        }
        order[source].resize(static_cast<std::size_t>(n));
        std::iota(order[source].begin(), order[source].end(), 0);
        std::sort(order[source].begin(), order[source].end(), [&](int first, int second){
            if(distance[source][first] != distance[source][second]){
                return distance[source][first] > distance[source][second];
            }
            return first < second;
        });
        const long long diameter = 2 * distance[source][order[source][0]];
        if(diameter < result.diameter){
            result.diameter = diameter;
            result.center_vertex = source;
        }
    }
    for(int index = 0; index < static_cast<int>(edges.size()); ++index){
        const auto& edge = edges[index];
        if(edge.from == edge.to) continue;
        const int u = edge.from, v = edge.to;
        int previous = order[u][0];
        for(int vertex: order[u]){
            if(distance[v][vertex] <= distance[v][previous]) continue;
            const long long diameter = distance[v][previous] + distance[u][vertex] + edge.weight;
            const long long offset = diameter - 2 * distance[u][vertex];
            if(offset >= 0 && offset <= 2 * edge.weight && diameter < result.diameter){
                result.diameter = diameter;
                result.center_vertex = -1;
                result.center_edge = index;
                result.center_offset_twice = offset;
            }
            previous = vertex;
        }
    }
    std::vector<std::pair<int, long long>> sources;
    if(result.center_edge == -1){
        sources.push_back({result.center_vertex, 0});
    }else{
        const auto& center = edges[result.center_edge];
        sources.push_back({center.from, result.center_offset_twice});
        sources.push_back({center.to, 2 * center.weight - result.center_offset_twice});
    }
    const auto parent = shortest_paths(sources, 2).second;
    for(int vertex = 0; vertex < n; ++vertex){
        if(result.center_edge == -1){
            if(vertex == result.center_vertex) continue;
        }else{
            const auto& center = edges[result.center_edge];
            if(vertex == center.from || vertex == center.to) continue;
        }
        result.edge_indices.push_back(parent[vertex]);
    }
    if(result.center_edge != -1) result.edge_indices.push_back(result.center_edge);
    return result;
}


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

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_MINIMUM_DIAMETER_SPANNING_TREE_HPP_INCLUDED
