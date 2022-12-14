// Copyright (c) 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/allocator/partition_allocator/partition_root.h"

#include <cstring>

#include "base/allocator/partition_allocator/partition_alloc_base/component_export.h"
#include "base/allocator/partition_allocator/partition_alloc_base/no_destructor.h"

namespace partition_alloc::internal {

namespace {
constexpr PartitionOptions kConfig{
    PartitionOptions::AlignedAlloc::kDisallowed,
    PartitionOptions::ThreadCache::kDisabled,
    PartitionOptions::Quarantine::kDisallowed,
    PartitionOptions::Cookie::kAllowed,
    PartitionOptions::BackupRefPtr::kDisabled,
    PartitionOptions::BackupRefPtrZapping::kDisabled,
    PartitionOptions::UseConfigurablePool::kNo,
};
}  // namespace

PA_COMPONENT_EXPORT(PARTITION_ALLOC)
ThreadSafePartitionRoot& PCScanMetadataAllocator() {
  static internal::base::NoDestructor<ThreadSafePartitionRoot> allocator(
      kConfig);
  return *allocator;
}

void ReinitPCScanMetadataAllocatorForTesting() {
  // First, purge memory owned by PCScanMetadataAllocator.
  PCScanMetadataAllocator().PurgeMemory(PurgeFlags::kDecommitEmptySlotSpans |
                                        PurgeFlags::kDiscardUnusedSystemPages);
  // Then, reinit the allocator.
  PCScanMetadataAllocator().~PartitionRoot();
  memset(&PCScanMetadataAllocator(), 0, sizeof(PCScanMetadataAllocator()));
  PCScanMetadataAllocator().Init(kConfig);
}

}  // namespace partition_alloc::internal
