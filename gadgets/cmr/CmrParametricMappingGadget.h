/** \file   CmrParametricMappingGadget.h
    \brief  This is the class gadget for cardiac parametric mapping, working on the mrd::ImageArray.
    \author Hui Xue
*/

#pragma once

#include "generic_recon_gadgets/GenericReconBase.h"

namespace Gadgetron {

#define GADGET_FAIL -1
#define GADGET_OK    0

    class CmrParametricMappingGadget : public GenericReconImageArrayBase
    {
    public:
        typedef GenericReconImageArrayBase BaseClass;

        CmrParametricMappingGadget(const Core::Context &context, const Core::GadgetProperties &properties);

        /// ------------------------------------------------------------------------------------
        /// parameters to control the mapping
        /// ------------------------------------------------------------------------------------

        NODE_PROPERTY(skip_processing_meta_field, std::string, "If this meta field exists, pass the incoming image array to next gadget without processing", GADGETRON_SKIP_PROCESSING_AFTER_RECON);

        NODE_PROPERTY(imaging_prep_time_from_protocol, bool, "If true, read in imaging prep time from protocols; if false, read in from meta fields", true);

        // -------------------------------------------

        NODE_PROPERTY(send_map, bool, "Whether to send out maps", true);
        NODE_PROPERTY(send_sd_map, bool, "Whether to send out sd maps", true);

        NODE_PROPERTY(color_lut_map, std::string, "Color lookup table for map", "GadgetronParametricMap.pal");
        NODE_PROPERTY(window_center_map, double, "Window center for map", 4.0);
        NODE_PROPERTY(window_width_map, double, "Window width for map", 8.0);

        NODE_PROPERTY(color_lut_map_3T, std::string, "Color lookup table for map at 3T", "GadgetronParametricMap_3T.pal");
        NODE_PROPERTY(window_center_map_3T, double, "Window center for map at 3T", 4.0);
        NODE_PROPERTY(window_width_map_3T, double, "Window width for map at 3T", 8.0);

        NODE_PROPERTY(scaling_factor_map, double, "Scale factor for map", 10.0);

        // -------------------------------------------

        NODE_PROPERTY(color_lut_sd_map, std::string, "Color lookup table for sd map", "GadgetronParametricSDMap.pal");
        NODE_PROPERTY(window_center_sd_map, double, "Window center for sd map", 4.0);
        NODE_PROPERTY(window_width_sd_map, double, "Window width for sd map", 8.0);
        NODE_PROPERTY(scaling_factor_sd_map, double, "Scale factor for sd map", 100.0);

        NODE_PROPERTY(perform_hole_filling, bool, "Whether to perform hole filling on map", true);
        NODE_PROPERTY(max_size_hole, int, "Maximal size for hole", 20);

        NODE_PROPERTY(std_thres_masking, double, "Number of noise std for masking", 3.0);
        NODE_PROPERTY(mapping_with_masking, bool, "Whether to compute and apply a mask for mapping", true);

        // ------------------------------------------------------------------------------------

    protected:

        // --------------------------------------------------
        // variables for protocol
        // --------------------------------------------------

        // field strength in T
        float field_strength_T_;

        // imaging prep time (e.g. inverison/saturation/echo time)
        std::vector<float> prep_times_;

        // encoding space size
        mrd::EncodingCounters meas_max_idx_;

        // --------------------------------------------------
        // functional functions
        // --------------------------------------------------

        // default interface function
        void process(Core::InputChannel<mrd::ImageArray>& in, Core::OutputChannel& out) override;

        // function to perform the mapping
        // data: input image array [RO E1 E2 CHA N S SLC]
        // map and map_sd: mapping result and its sd
        // para and para_sd: other parameters of mapping and its sd
        virtual int perform_mapping(mrd::ImageArray& data, mrd::ImageArray& map, mrd::ImageArray& para, mrd::ImageArray& map_sd, mrd::ImageArray& para_sd) = 0;

        // fill image header and meta for maps
        virtual int fill_map_header(mrd::ImageArray& map);
        virtual int fill_sd_header(mrd::ImageArray& map_sd);

        // compute image mask
        virtual void compute_mask_for_mapping(const hoNDArray<float> &mag, hoNDArray<float> &mask,
                                              float scale_factor);
    };
}
