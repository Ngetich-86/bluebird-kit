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
/*
 * Approach: one pass over the rows into an open-addressing hash table keyed
 * by request ID (linear probing), then one pass over the table to collect
 * flagged IDs.
 *   - Occupancy is a separate flag, so every 64-bit value (0, UINT64_MAX)
 *     is a valid key; there is no sentinel key.
 *   - IDs are mixed with the murmur3 fmix64 finalizer before masking, so IDs
 *     that differ only in high bits do not pile into one probe chain.
 *   - neg/pos are saturating counters capped at 2: "2" means "2 or more",
 *     so a large group can never wrap back to look like "exactly one".
 *   - Capacity is a power of two >= 2 * (max distinct IDs), allocated once:
 *     load factor <= 0.5, so probe chains stay short and always terminate.
 *   - The table is the only allocation (one calloc), sized from n.
 */
#define MAX_DISTINCT 5000000u
#define PREFETCH_DIST 16

typedef struct {
    uint64_t key;
    int64_t sum;   /* |sum| <= 10^18, fits in int64 per the README bounds */
    uint8_t used;
    uint8_t neg;   /* saturates at 2 */
    uint8_t pos;   /* saturates at 2 */
} Slot;

static inline uint64_t mix64(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

static size_t reconcile(size_t n, const uint64_t *req_id, const int32_t *acct,
                        const int64_t *delta, int32_t M, int64_t *net,
                        uint64_t *bad) {
    (void)M; /* acct[i] is guaranteed to be in [0, M) */
    for (size_t i = 0; i < n; i++) net[acct[i]] += delta[i];
    if (n == 0) return 0;

    size_t distinct_max = n < MAX_DISTINCT ? n : MAX_DISTINCT;
    size_t cap = 16;
    while (cap < 2 * distinct_max) cap <<= 1;
    size_t mask = cap - 1;
    Slot *table = calloc(cap, sizeof *table);
    if (!table) { fprintf(stderr, "error: out of memory\n"); exit(2); }

    for (size_t i = 0; i < n; i++) {
#if defined(__GNUC__) || defined(__clang__)
        /* Rows are in random order, so nearly every probe is a cache miss;
         * start loading the slot for a row a few iterations ahead. */
        if (i + PREFETCH_DIST < n)
            __builtin_prefetch(&table[mix64(req_id[i + PREFETCH_DIST]) & mask], 1);
#endif
        uint64_t k = req_id[i];
        size_t h = (size_t)mix64(k) & mask;
        while (table[h].used && table[h].key != k) h = (h + 1) & mask;
        Slot *s = &table[h];
        if (!s->used) { s->used = 1; s->key = k; }
        s->sum += delta[i];
        if (delta[i] < 0) { if (s->neg < 2) s->neg++; }
        else              { if (s->pos < 2) s->pos++; }
    }

    size_t nbad = 0;
    for (size_t h = 0; h < cap; h++) {
        const Slot *s = &table[h];
        if (s->used && !(s->neg == 1 && s->pos == 1 && s->sum == 0))
            bad[nbad++] = s->key;
    }
    free(table);
    return nbad;
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
