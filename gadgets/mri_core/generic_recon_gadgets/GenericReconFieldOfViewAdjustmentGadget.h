/** \file   GenericReconFieldOfViewAdjustmentGadget.h
    \brief  This is the class gadget for both 2DT and 3DT reconstruction, working on the ImageArray.
            This gadget will adjust the image field-of-view and/or image size to the protocol prescribed values.

            This class is a part of general cartesian recon chain.

\author     Hui Xue
*/

#pragma once

#include "GenericReconBase.h"

namespace Gadgetron {

    class GenericReconFieldOfViewAdjustmentGadget : public GenericReconImageArrayBase
    {
    public:
        typedef GenericReconImageArrayBase BaseClass;

        GenericReconFieldOfViewAdjustmentGadget(const Core::Context &context, const Core::GadgetProperties &properties);
        ~GenericReconFieldOfViewAdjustmentGadget() override;

    protected:

        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        // encoding FOV and recon FOV
        std::vector< std::vector<float> > encoding_FOV_;
        std::vector< std::vector<float> > recon_FOV_;
        // recon size
        std::vector< std::vector<size_t> > recon_size_;

        // --------------------------------------------------
        // variable for recon
        // --------------------------------------------------

        // kspace filter
        hoNDArray< std::complex<float> > filter_RO_;
        hoNDArray< std::complex<float> > filter_E1_;
        hoNDArray< std::complex<float> > filter_E2_;

        // kspace buffer
        hoNDArray< std::complex<float> > kspace_buf_;

        // results of filtering
        hoNDArray< std::complex<float> > res_;

        // --------------------------------------------------
        // functional functions
        // --------------------------------------------------

        // default interface function
        void process(Core::InputChannel< mrd::ImageArray >& in, Core::OutputChannel& out) override;

        // adjust FOV
        int adjust_FOV(mrd::ImageArray& data);

        // perform fft or ifft
        void perform_fft(size_t E2, const hoNDArray< std::complex<float> >& input, hoNDArray< std::complex<float> >& output);
        void perform_ifft(size_t E2, const hoNDArray< std::complex<float> >& input, hoNDArray< std::complex<float> >& output);
    };
}
