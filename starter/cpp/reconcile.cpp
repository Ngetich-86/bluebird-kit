// Imperium | Blue Bird - request reconciliation exercise (C++ starter)
//
// Build:  c++ -O2 -std=c++17 -o reconcile reconcile.cpp
// Run:    ./reconcile < ../../tests/cases/01_basic.in
// Test:   python3 ../../tests/run_tests.py -- ./reconcile
//
// Input parsing and output printing are done for you. Implement reconcile().
// Rules (full statement and bounds in README.md): single-threaded; no heap
// allocation per row; no std::unordered_map / std::map or other library hash
// map (write your own table, or sort/partition); request IDs 0 and
// UINT64_MAX are valid.
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

// Inputs:  n rows; req_id[i], acct[i] in [0, M), delta[i] != 0.
// Outputs: net (size M, zeroed) gets each account's summed deltas.
//          Write each flagged request ID once into bad (capacity n, any order)
//          and return the count. A request is OK only with exactly one negative
//          row and one positive row summing to zero.
static size_t reconcile(size_t n, const uint64_t* req_id, const int32_t* acct,
                        const int64_t* delta, int32_t M, int64_t* net,
                        uint64_t* bad) {
    (void)n; (void)req_id; (void)acct; (void)delta; (void)M; (void)net; (void)bad;
    // TODO: your implementation
    return 0;
}

// ---- I/O harness: you should not need to change anything below. ----
static void die(const char* msg) { std::fprintf(stderr, "error: %s\n", msg); std::exit(2); }

int main() {
    size_t n; long long m;
    if (std::scanf("%zu %lld", &n, &m) != 2) die("bad header (expected: n M)");
    if (m < 1 || m > 100000) die("M out of range");
    const int32_t M = static_cast<int32_t>(m);
    std::vector<uint64_t> req_id(n ? n : 1), bad(n ? n : 1);
    std::vector<int32_t> acct(n ? n : 1);
    std::vector<int64_t> delta(n ? n : 1), net(M, 0);
    for (size_t i = 0; i < n; i++) {
        if (std::scanf("%" SCNu64 " %" SCNd32 " %" SCNd64, &req_id[i], &acct[i], &delta[i]) != 3)
            die("bad row");
    }
    size_t nbad = reconcile(n, req_id.data(), acct.data(), delta.data(), M, net.data(), bad.data());
    for (int32_t a = 0; a < M; a++) std::printf("%" PRId64 "\n", net[a]);
    std::printf("BAD %zu\n", nbad);
    for (size_t i = 0; i < nbad; i++) std::printf("%" PRIu64 "\n", bad[i]);
    return 0;
}
