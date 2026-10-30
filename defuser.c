#include "defuser.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_FIELDS 8

static int write_answer(
    char *out,
    size_t cap,
    const uint32_t *values,
    size_t count
) {
    size_t used = 0;

    if (out == NULL || values == NULL || count == 0) {
        return 1;
    }

    for (size_t i = 0; i < count; ++i) {
        if (used >= cap) {
            return 1;
        }

        int size = snprintf(
            out + used,
            cap - used,
            "%s%" PRIu32,
            i ? " " : "",
            values[i]
        );

        if (size < 0 || (size_t)size >= cap - used) {
            return 1;
        }

        used += (size_t)size;
    }

    return 0;
}

static uint32_t rotate_left_32(uint32_t value, unsigned int shift) {
    shift &= 31u;
    return (value << shift) | (value >> ((32u - shift) & 31u));
}

static uint32_t rotate_right_32(uint32_t value, unsigned int shift) {
    shift &= 31u;
    return (value >> shift) | (value << ((32u - shift) & 31u));
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};

    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t middle = (uint32_t)(challenge >> 16);

    uint32_t a = (5u * low + high) ^ 0x4ec2u;
    uint32_t b = (middle + 0xfcd4u) ^ high;
    uint32_t c = (5u * b) ^ (low + 0xd405u);

    values[0] = c ^ a;

    return write_answer(out, cap, values, 1);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};

    values[0] = 3u * (uint32_t)((challenge >> 45) & 0x3fu);

    for (size_t i = 1; i < 6; ++i) {
        uint32_t term =
            (uint32_t)((challenge >> (5u * (unsigned int)i)) & 0x1fu);

        values[i] =
            (values[i - 1] + term) ^ (uint32_t)(6u * (uint32_t)i);
    }

    return write_answer(out, cap, values, 6);
}

static uint32_t phase_3_dispatch(uint32_t index, uint32_t value) {
    switch (index) {
        case 0:
            return 3u * value + 0x142eu;
        case 1:
            return 2u * value ^ 0x9b32u;
        case 2:
            return 5u * value + 0x1eeecu;
        case 3:
            return 2u * value ^ 0xf165u;
        case 4:
            return 5u * value + 0x4a696u;
        case 5:
            return (value ^ 0x95bdu) + (value >> 2);
        case 6:
            return (value ^ 0x80d0u) + (value >> 1);
        case 7:
            return 5u * value + 0x7ec5u;
        default:
            return 0;
    }
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};

    uint32_t index = (uint32_t)challenge & 7u;
    uint32_t argument =
        (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);

    values[0] = index;
    values[1] = phase_3_dispatch(index, argument);

    return write_answer(out, cap, values, 2);
}

static uint32_t phase_4_walk(
    uint32_t target,
    uint32_t low,
    uint32_t high,
    uint32_t key
) {
    uint32_t middle = low + ((high - low) >> 1);

    if (target == middle) {
        return (middle + 3u) ^ key;
    }

    uint32_t branch_key = middle + key;
    uint32_t result;

    if (target < middle) {
        result = phase_4_walk(target, low, middle - 1u, key);
        return ((3u * result) ^ branch_key) ^ 0x0au;
    }

    result = phase_4_walk(target, middle + 1u, high, key);
    return ((3u * result) ^ branch_key) ^ 0x14u;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};

    uint32_t target = (uint32_t)challenge & 0x7fu;
    uint32_t key =
        (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);

    values[0] = phase_4_walk(target, 0, 0x7fu, key);

    return write_answer(out, cap, values, 1);
}

typedef struct Phase5Node {
    unsigned int next;
    uint32_t value;
    uint32_t addend;
} Phase5Node;

static const Phase5Node phase_5_nodes[] = {
    {7, 0x0230u, 0x59b0u},
    {6, 0x0034u, 0x8ecbu},
    {5, 0x056du, 0xcb59u},
    {2, 0x0719u, 0xdcf6u},
    {1, 0x0944u, 0xb60fu},
    {9, 0x0a2cu, 0x25c3u},
    {11, 0x0632u, 0xe3e5u},
    {10, 0x082bu, 0x0db0u},
    {0, 0x0464u, 0x63e5u},
    {9, 0x0b43u, 0x04b4u},
    {3, 0x0141u, 0x5a66u},
    {8, 0x034du, 0x8e64u}
};

static const unsigned int phase_5_heads[8] = {
    4, 1, 6, 11, 8, 0, 7, 10
};

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};

    unsigned int node_index =
        phase_5_heads[(unsigned int)(challenge & 7u)];

    unsigned int steps =
        (unsigned int)((challenge >> 3) & 3u);

    uint32_t state =
        (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);

    state ^= 0xf6b1u;
    state = 3u * state + phase_5_nodes[node_index].addend;
    state ^= phase_5_nodes[node_index].value;

    for (unsigned int i = 0; i < steps; ++i) {
        node_index = phase_5_nodes[node_index].next;
        state = 3u * state + phase_5_nodes[node_index].addend;
        state ^= phase_5_nodes[node_index].value;
    }

    values[0] = state;
    values[1] = phase_5_nodes[node_index].value;

    return write_answer(out, cap, values, 2);
}

static uint32_t phase_6_operation(
    unsigned int operation,
    uint32_t value
) {
    switch (operation) {
        case 0:
            return rotate_left_32(value, 5) ^ 0xa23fu;
        case 1:
            return rotate_right_32(value, 2);
        case 2:
            return (value ^ 0xa3dfu) + (value << 5);
        case 3:
            return (value + 0xccc2u) ^ (value >> 2);
        case 4:
            return (8u * value + 0xa960u) ^ (5u * value);
        case 5:
            return (value >> 3) ^ value ^ 0x17bfu;
        default:
            return 0;
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    static const unsigned int operation_table[8] = {
        3, 4, 1, 0, 1, 2, 5, 0
    };

    uint32_t values[MAX_FIELDS] = {0};

    unsigned int first_index =
        (unsigned int)(challenge & 7u);

    unsigned int second_index =
        (unsigned int)((challenge >> 3) & 7u);

    uint32_t state =
        (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);

    state = phase_6_operation(operation_table[first_index], state);
    state ^= 0x7387u;
    state = phase_6_operation(operation_table[second_index], state);
    state += 0x113au;

    values[0] = state;

    return write_answer(out, cap, values, 1);
}

static uint32_t secret_mix(uint32_t value) {
    return (5u * value + 0x45edu) ^ 0x603fu;
}

int solve_secret_phase(
    uint64_t challenge,
    uint64_t context,
    char *out,
    size_t cap
) {
    uint32_t values[3] = {0};

    uint32_t index = (uint32_t)context & 3u;
    uint32_t challenge_fold =
        (uint32_t)challenge ^ (uint32_t)(challenge >> 32);

    uint32_t context_fold =
        (uint32_t)context ^ (uint32_t)(context >> 32);

    uint32_t mixed =
        secret_mix(challenge_fold ^ context_fold);

    uint32_t second =
        mixed ^
        (index * 0x45edu) ^
        (uint32_t)(context >> 16);

    uint32_t third =
        mixed ^
        (context_fold + index + 0x603fu + (challenge_fold >> 5));

    values[0] = index;
    values[1] = second;
    values[2] = third;

    size_t used = 0;
    int size = snprintf(out, cap, "OPEN");

    if (size < 0 || (size_t)size >= cap) {
        return 1;
    }

    used = (size_t)size;

    for (size_t i = 0; i < 3; ++i) {
        size = snprintf(
            out + used,
            cap - used,
            " %" PRIu32,
            values[i]
        );

        if (size < 0 || (size_t)size >= cap - used) {
            return 1;
        }

        used += (size_t)size;
    }

    return 0;
}
