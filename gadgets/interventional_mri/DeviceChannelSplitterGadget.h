#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include <complex>

namespace Gadgetron{

  template <typename T> class DeviceChannelSplitterGadget :
  public Core::ChannelGadget<mrd::Image<T>>
  {
    using Core::ChannelGadget<mrd::Image<T>>::ChannelGadget;
  protected:
    void process(Core::InputChannel<mrd::Image<T>>& in, Core::OutputChannel& out) override;
  };

  class DeviceChannelSplitterGadgetUSHORT :
  public DeviceChannelSplitterGadget<uint16_t>
  {
    using DeviceChannelSplitterGadget<uint16_t>::DeviceChannelSplitterGadget;
  };

  class DeviceChannelSplitterGadgetFLOAT :
  public DeviceChannelSplitterGadget<float>
  {
    using DeviceChannelSplitterGadget<float>::DeviceChannelSplitterGadget;
  };

  class DeviceChannelSplitterGadgetCPLX :
  public DeviceChannelSplitterGadget< std::complex<float> >
  {
    using DeviceChannelSplitterGadget< std::complex<float> >::DeviceChannelSplitterGadget;
  };
}

