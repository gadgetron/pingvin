/**
    \brief  Sorts all pipeline images by the selected sorting dimension flag
    \test   Tested by: distributed_simple_gre.cfg, distributed_buffer_simple_gre.cfg
*/

#pragma once

#include "MRNode.h"
#include "hoNDArray.h"

namespace Gadgetron{

  struct ImageEntry
  {
    mrd::AnyImage image_;
    int index_;
  };

  class ImageSortGadget : public Core::MRChannelGadget<mrd::AnyImage> {
    public:
      struct Parameters : public Core::NodeParameters {
        std::string sorting_dimension = "slice";
        Parameters(const std::string &prefix) : Core::NodeParameters(prefix, "Image Sorting Options") {
          register_parameter("sorting_dimension", &sorting_dimension, "Dimension that data will be sorted by (average, slice, contrast, phase, repetition, set)");
        }
      };

      ImageSortGadget(const Core::MRContext& context, const Parameters& params)
        : ImageSortGadget::MRChannelGadget(context, params)
        , parameters_(params)
      {}

      void process(Core::InputChannel<mrd::AnyImage> &, Core::OutputChannel &) override;

    protected:
      const Parameters parameters_;
      std::vector<ImageEntry> images_;
  };
}