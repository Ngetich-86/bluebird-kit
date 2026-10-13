/*
 * Imperium | Blue Bird - request reconciliation exercise (C starter)
 *
 * Build:  cc -O2 -std=c11 -o reconcile reconcile.c
 * Run:    ./reconcile < ../../tests/cases/01_basic.in
 * Test:   python3 ../../tests/run_tests.py -- ./reconcile
 *
 * Input parsing and output printing are done for you. Implement reconcile()
 * below. Rules (see README.md for the full statement and bounds):
 *   - single-threaded;
 *   - no heap allocation per row (allocate up front, sized from n and M);
 *   - no library hash map: write your own table, or sort/partition;
 *   - request ID 0 and UINT64_MAX are valid IDs.
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Inputs:  n rows; req_id[i], acct[i] in [0, M), delta[i] != 0.
 * Outputs: net[0..M-1] is zeroed by the caller; add each account's deltas.
 *          Write each flagged request ID once into bad[] (capacity n, any
 *          order) and return how many you wrote.
 * A request is OK only if it has exactly one negative row and exactly one
 * positive row, and their deltas sum to zero. Every other request is flagged.
 */
static size_t reconcile(size_t n, const uint64_t *req_id, const int32_t *acct,
                        const int64_t *delta, int32_t M, int64_t *net,
                        uint64_t *bad) {
    (void)n; (void)req_id; (void)acct; (void)delta; (void)M; (void)net; (void)bad;
    /* TODO: your implementation */
    return 0;
}

/* ---- I/O harness: you should not need to change anything below. ---- */
static void die(const char *msg) { fprintf(stderr, "error: %s\n", msg); exit(2); }

int main(void) {
    size_t n; long long m;
    if (scanf("%zu %lld", &n, &m) != 2) die("bad header (expected: n M)");
    if (m < 1 || m > 100000) die("M out of range");
    int32_t M = (int32_t)m;
    uint64_t *req_id = malloc((n ? n : 1) * sizeof *req_id);
    int32_t *acct = malloc((n ? n : 1) * sizeof *acct);
    int64_t *delta = malloc((n ? n : 1) * sizeof *delta);
    int64_t *net = calloc((size_t)M, sizeof *net);
    uint64_t *bad = malloc((n ? n : 1) * sizeof *bad);
    if (!req_id || !acct || !delta || !net || !bad) die("out of memory");
    for (size_t i = 0; i < n; i++) {
        if (scanf("%" SCNu64 " %" SCNd32 " %" SCNd64, &req_id[i], &acct[i], &delta[i]) != 3)
            die("bad row");
    }
    size_t nbad = reconcile(n, req_id, acct, delta, M, net, bad);
    for (int32_t a = 0; a < M; a++) printf("%" PRId64 "\n", net[a]);
    printf("BAD %zu\n", nbad);
    for (size_t i = 0; i < nbad; i++) printf("%" PRIu64 "\n", bad[i]);
    return 0;
}
