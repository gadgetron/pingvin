#pragma once

#include "SliceAccumulator.h"

#include "MRParallel.h"

namespace Gadgetron::Grappa {
    using AcquisitionFanout = Core::Parallel::Fanout<Slice>;
}