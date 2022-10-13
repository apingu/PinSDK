// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/containers/span_rust.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace {

TEST(BaseSpanRustTest, SliceConstruct) {
  uint8_t data[] = {0, 1, 2, 3, 4};
  span<const uint8_t> data_span(data, 2);
  rust::Slice<const uint8_t> rust_slice = SpanToRustSlice(data_span);
  EXPECT_EQ(2ul, rust_slice.length());
  EXPECT_EQ(1, rust_slice[1]);
}

}  // namespace
}  // namespace base
