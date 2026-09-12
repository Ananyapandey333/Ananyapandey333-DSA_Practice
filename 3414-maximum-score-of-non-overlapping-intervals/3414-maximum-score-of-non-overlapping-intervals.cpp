#include <vector>
#include <algorithm>

using namespace std;

struct Interval {
    int l, r, weight, id;
};

class Solution {
public:
    vector<int> maximumWeight(vector<vector<int>>& intervals) {
        int n = intervals.size();
        vector<Interval> sortedIntervals(n);
        for (int i = 0; i < n; ++i) {
            sortedIntervals[i] = {intervals[i][0], intervals[i][1], intervals[i][2], i};
        }

        // Sort by end time r
        sort(sortedIntervals.begin(), sortedIntervals.end(), [](const Interval& a, const Interval& b) {
            if (a.r != b.r) return a.r < b.r;
            return a.l < b.l;
        });

        // Store end times for binary search
        vector<int> endTimes(n);
        for (int i = 0; i < n; ++i) {
            endTimes[i] = sortedIntervals[i].r;
        }

        // DP table: dp[k][i] = pair<total_weight, vector<indices>>
        // k from 0 to 4, i from 0 to n
        using DPState = pair<long long, vector<int>>;
        vector<vector<DPState>> dp(5, vector<DPState>(n + 1, {0, {}}));

        for (int i = 1; i <= n; ++i) {
            const auto& curr = sortedIntervals[i - 1];
            
            // Find the last interval that ends strictly before curr.l
            // upper_bound - 1 gives the index of the last element < curr.l
            int prevIdx = upper_bound(endTimes.begin(), endTimes.begin() + i - 1, curr.l - 1) - endTimes.begin();

            for (int k = 1; k <= 4; ++k) {
                // Option 1: Do not include current interval
                DPState bestOption = dp[k][i - 1];

                // Option 2: Include current interval
                long long newWeight = dp[k - 1][prevIdx].first + curr.weight;
                vector<int> newIndices = dp[k - 1][prevIdx].second;
                newIndices.push_back(curr.id);
                sort(newIndices.begin(), newIndices.end()); // Keep indices sorted internally

                DPState includeOption = {newWeight, newIndices};

                // Compare Option 1 and Option 2
                if (includeOption.first > bestOption.first) {
                    bestOption = includeOption;
                } else if (includeOption.first == bestOption.first) {
                    if (bestOption.first == 0) {
                        // Keep bestOption as is if weight is 0
                    } else if (bestOption.second.empty() || includeOption.second < bestOption.second) {
                        bestOption = includeOption;
                    }
                }

                dp[k][i] = bestOption;
            }
        }

        // Return the best vector of indices among choosing 1 to 4 intervals
        DPState result = {0, {}};
        for (int k = 1; k <= 4; ++k) {
            if (dp[k][n].first > result.first) {
                result = dp[k][n];
            } else if (dp[k][n].first == result.first) {
                if (result.second.empty() || dp[k][n].second < result.second) {
                    result = dp[k][n];
                }
            }
        }

        return result.second;
    }
};