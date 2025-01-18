#pragma once

#include "Node.h"
#include "hoNDArray.h"
#include "hoNDKLT.h"

#include <complex>
#include <map>

namespace Gadgetron {

  class PCACoilGadget : public Core::MRChannelGadget<mrd::Acquisition>
  {
  public:
    struct Parameters : public Core::NodeParameters {
      std::string uncombined_channels_by_name = "";

      Parameters(const std::string& prefix) : Core::NodeParameters(prefix, "PCA Coil Options") {
        register_parameter("uncombined-channels-by-name", &uncombined_channels_by_name, "List of comma separated channels by name");
      }
    };

    PCACoilGadget(const Core::MrdContext& context, const Parameters& params);
    ~PCACoilGadget() override;

    void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& output) override;

  protected:
    const Parameters parameters_;

    void calculate_coefficients(int location);
    void do_pca(mrd::Acquisition& acq);

    std::vector<unsigned int> uncombined_channels_;

    //Map containing buffers, one for each location
    std::map< int, std::vector< mrd::Acquisition > > buffer_;

    //Keep track of whether we are buffering for a particular location
    std::map< int, bool> buffering_mode_;

    //Map for storing PCA coefficients for each location
    std::map<int, hoNDKLT<std::complex<float> >* > pca_coefficients_;

    const size_t max_buffered_profiles_ = 100;
    const size_t samples_to_use_ = 16;
  };
}