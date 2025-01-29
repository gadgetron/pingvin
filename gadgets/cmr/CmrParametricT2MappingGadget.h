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

        CmrParametricT2MappingGadget(const Core::Context& context, const Core::GadgetProperties& properties);

        /// ------------------------------------------------------------------------------------
        /// parameters to control the mapping
        /// ------------------------------------------------------------------------------------

        NODE_PROPERTY(max_iter, size_t, "Maximal number of iterations", 150);
        NODE_PROPERTY(thres_func, double, "Threshold for minimal change of cost function", 1e-4);
        NODE_PROPERTY(max_T2, double, "Maximal T2 allowed in mapping (ms)", 4000);

    protected:

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
