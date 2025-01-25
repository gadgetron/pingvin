#include "FFTXGadget.h"
#include "hoNDFFT.h"
#include "hoNDArray_utils.h"

namespace Gadgetron{

  void FFTXGadget::process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out)
  {
    hoNDArray< std::complex<float> > r_;
    hoNDArray< std::complex<float> > buf_;

    for (auto acq : input) {
      auto& data = acq.data;

      // FFT along 1st dimensions (x)
      if (buf_.get_size(0) != data.get_size(0)) {
        buf_ = data;
      }

      hoNDFFT<float>::instance()->fft1c(data, r_, buf_);
      memcpy(data.begin(), r_.begin(), r_.get_number_of_bytes());

      out.push(std::move(acq));
    }
  }

  GADGETRON_GADGET_EXPORT(FFTXGadget)
}
