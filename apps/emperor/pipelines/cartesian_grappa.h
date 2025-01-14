#pragma once

#include "pipeline.h"

#include "gadgets/mri_core/NoiseAdjustGadget.h"
#include "gadgets/mri_core/AsymmetricEchoAdjustROGadget.h"
#include "gadgets/mri_core/RemoveROOversamplingGadget.h"
#include "gadgets/mri_core/AcquisitionAccumulateTriggerGadget.h"
#include "gadgets/mri_core/BucketToBufferGadget.h"
#include "gadgets/mri_core/ImageArraySplitGadget.h"
#include "gadgets/mri_core/ComplexToFloatGadget.h"
#include "gadgets/mri_core/FloatToFixPointGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianReferencePrepGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconEigenChannelGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconCartesianGrappaGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconPartialFourierHandlingFilterGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconKSpaceFilteringGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconFieldOfViewAdjustmentGadget.h"
#include "gadgets/mri_core/generic_recon_gadgets/GenericReconImageArrayScalingGadget.h"

namespace pingvin {

class CartesianGrappa : public Pipeline {
  public:
    CartesianGrappa() : Pipeline("cartesian-grappa", "Cartesian Grappa Recon") { }

    void build(void) override {
        this->append<Gadgetron::NoiseAdjustGadget>();
        this->append<Gadgetron::AsymmetricEchoAdjustROGadget>();
        this->append<Gadgetron::RemoveROOversamplingGadget>();
        this->append<Gadgetron::AcquisitionAccumulateTriggerGadget>();
        this->append<Gadgetron::BucketToBufferGadget>();
        this->append<Gadgetron::GenericReconCartesianReferencePrepGadget>();
        this->append<Gadgetron::GenericReconEigenChannelGadget>();
        this->append<Gadgetron::GenericReconCartesianGrappaGadget>();
        this->append<Gadgetron::GenericReconPartialFourierHandlingFilterGadget>();
        this->append<Gadgetron::GenericReconKSpaceFilteringGadget>();
        this->append<Gadgetron::GenericReconFieldOfViewAdjustmentGadget>();
        this->append<Gadgetron::GenericReconImageArrayScalingGadget>();
        this->append<Gadgetron::ImageArraySplitGadget>();
        this->append<Gadgetron::ComplexToFloatGadget>();
        this->append<Gadgetron::FloatToUShortGadget>();
    }
};

} // namespace pingvin