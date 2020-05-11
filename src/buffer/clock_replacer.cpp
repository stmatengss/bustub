//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"

namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
    clock_hand = 0;
    num_pages_cr = num_pages;
    ref_flag = std::vector<bool>(num_pages, false);
    in_cr = std::vector<bool>(num_pages, false);
    cr_size = 0;
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
    std::lock_guard<std::mutex> lock(cr_lk);
    for ( size_t i = 0; i < num_pages_cr; i++ ) {
        if (in_cr[clock_hand]) {
            if (ref_flag[clock_hand]) {
                ref_flag[clock_hand] = false;
            } else {
                cr_size--;
                in_cr[clock_hand] = false;
                ref_flag[clock_hand] = false;
                *frame_id = clock_hand;
                clock_hand = (clock_hand + 1) % num_pages_cr;
                return true;
            }
        }
        clock_hand = (clock_hand + 1) % num_pages_cr;
    }

    return false;
}

void ClockReplacer::Pin(frame_id_t frame_id) {
    std::lock_guard<std::mutex> lock(cr_lk);
    if (in_cr[frame_id]) {
        ref_flag[frame_id] = false;
        in_cr[frame_id] = false;
        cr_size--;
    }
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
    std::lock_guard<std::mutex> lock(cr_lk);
    if (!in_cr[frame_id]) {
        ref_flag[frame_id] = true;
        in_cr[frame_id] = true;
        cr_size++;
    }
}

size_t ClockReplacer::Size() {
    return cr_size;
}

}  // namespace bustub
