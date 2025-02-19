/**
\file   CmrParametricT2MappingGadget.h
\brief  This is the class gadget for cardiac T2 mapping, working on the mrd::ImageArray.
\author Hui Xue
*/

#pragma once

#include "CmrParametricMappingGadget.h"

namespace Gadgetron {

    class CmrParametricT2MappingGadget : public CmrParametricMappingGadget
    {
    public:
        typedef CmrParametricMappingGadget BaseClass;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix): BaseClass::Parameters(prefix, "T2 Mapping") {
                register_parameter("max-iter", &max_iter, "Maximal number of iterations");
                register_parameter("thres-func", &thres_func, "Threshold for minimal change of cost function");
                register_parameter("max-T2", &max_T2, "Maximal T2 allowed in mapping (ms)");
            }

            size_t max_iter = 150;
            double thres_func = 1e-4;
            double max_T2 = 4000;
        };

        CmrParametricT2MappingGadget(const Core::MRContext& context, const Parameters& params);

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
