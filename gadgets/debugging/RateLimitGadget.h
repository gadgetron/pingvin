#pragma once

#include "Node.h"
#include "hoNDArray.h"

namespace Gadgetron{

    class RateLimitGadget : public Core::MRChannelGadget<mrd::StreamItem>
    {
    public:
      struct Parameters : public Core::NodeParameters {
        Parameters(const std::string& prefix): Core::NodeParameters(prefix, "Rate Limiter") {
          register_parameter("sleep-time", &sleep_time_int, "Sleep time in milliseconds");
        }

        int sleep_time_int = 0;
      };

      RateLimitGadget(const Core::MrdContext& context, const Parameters& params);

      void process(Core::InputChannel<mrd::StreamItem>& input, Core::OutputChannel& output) override;

    protected:
      const Parameters params_;

      std::chrono::milliseconds sleep_time_;
    };
}
