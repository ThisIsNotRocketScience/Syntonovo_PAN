#pragma once

typedef union _midicmd_t {
        struct {
                uint8_t header;
                uint8_t b1;
                uint8_t b2;
                uint8_t b3;
        };

        uint32_t word;
} midicmd_t;
