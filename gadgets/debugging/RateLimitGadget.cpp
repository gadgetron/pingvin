#include "RateLimitGadget.h"

#include <thread>
#include <chrono>

namespace Gadgetron {

RateLimitGadget::RateLimitGadget(const Core::MrdContext& context, const Parameters& params)
    : Core::MRChannelGadget<mrd::StreamItem>(context, params)
    , params_(params)
{
  this->sleep_time_ = std::chrono::milliseconds(params_.sleep_time_int);
}

void RateLimitGadget::process(Core::InputChannel<mrd::StreamItem>& in, Core::OutputChannel& out)
{
  for (auto item : in) {
    std::this_thread::sleep_for(this->sleep_time_);
    out.push(item);
  }
}

GADGETRON_GADGET_EXPORT(RateLimitGadget)
} // namespace Gadgetron
