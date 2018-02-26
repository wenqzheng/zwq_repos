// lockfree_queue.h
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "readerwriterqueue.h"
#include "concurrentqueue.h"
#include "blockingconcurrentqueue.h"

using spsc_queue = moodycamel::ReaderWriterQueue;
using block_spsc_queue = moodycamel::BlockingReaderWriterQueue;
using mpmc_queue = moodycamel::moodycamel::ConcurrentQueue;
using block_mpmc_queue = moodycamel::BlockingConcurrentQueue;
