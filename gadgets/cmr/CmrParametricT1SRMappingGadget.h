/**
\file   CmrParametricT1SRMappingGadget.h
\brief  This is the class gadget for cardiac T1 SR mapping, working on the mrd::ImageArray.
\author Hui Xue
*/

#pragma once

#include "CmrParametricMappingGadget.h"

namespace Gadgetron {

    class CmrParametricT1SRMappingGadget : public CmrParametricMappingGadget
    {
    public:
        typedef CmrParametricMappingGadget BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix): BaseClass::Parameters(prefix, "T1SR Mapping") {
                register_parameter("max-iter", &max_iter, "Maximal number of iterations");
                register_parameter("thres-func", &thres_func, "Threshold for minimal change of cost function");
                register_parameter("max-T1", &max_T1, "Maximal T1 allowed in mapping (ms)");

                register_parameter("anchor-image-index", &anchor_image_index, "Index for anchor image; by default, the first image is the anchor (without SR pulse)");
                register_parameter("anchor-TS", &anchor_TS, "Saturation time for anchor");
            }

            size_t max_iter = 150;
            double thres_func = 1e-4;
            double max_T1 = 4000;

            size_t anchor_image_index = 0;
            double anchor_TS = 10000;
        };

        CmrParametricT1SRMappingGadget(const Core::MRContext& context, const Parameters& params);

    protected:
        const Parameters params_;

        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        // function to perform the mapping
        // data: input image array [RO E1 E2 CHA N S SLC]
        // map and map_sd: mapping result and its sd
        // para and para_sd: other parameters of mapping and its sd
        int perform_mapping(mrd::ImageArray& data, mrd::ImageArray& map, mrd::ImageArray& para, mrd::ImageArray& map_sd, mrd::ImageArray& para_sd) override;
    };
}
