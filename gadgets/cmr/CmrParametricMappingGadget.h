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

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix): Parameters(prefix, "CmrParametricMapping")
            {}

            Parameters(const std::string& prefix, const std::string& description): BaseClass::Parameters(prefix, description) {
                register_parameter("skip-processing-meta-field", &skip_processing_meta_field, "If this meta field exists, pass the incoming image array to next gadget without processing");
                register_parameter("imaging-prep-time-from-protocol", &imaging_prep_time_from_protocol, "If true, read in imaging prep time from protocols; if false, read in from meta fields");

                register_parameter("send-map", &send_map, "Whether to send out maps");
                register_parameter("send-sd-map", &send_sd_map, "Whether to send out sd maps");

                register_parameter("color-lut-map", &color_lut_map, "Color lookup table for map");
                register_parameter("window-center-map", &window_center_map, "Window center for map");
                register_parameter("window-width-map", &window_width_map, "Window width for map");

                register_parameter("color-lut-map-3T", &color_lut_map_3T, "Color lookup table for map at 3T");
                register_parameter("window-center-map-3T", &window_center_map_3T, "Window center for map at 3T");
                register_parameter("window-width-map-3T", &window_width_map_3T, "Window width for map at 3T");

                register_parameter("scaling-factor-map", &scaling_factor_map, "Scale factor for map");

                register_parameter("color-lut-sd-map", &color_lut_sd_map, "Color lookup table for sd map");
                register_parameter("window-center-sd-map", &window_center_sd_map, "Window center for sd map");
                register_parameter("window-width-sd-map", &window_width_sd_map, "Window width for sd map");
                register_parameter("scaling-factor-sd-map", &scaling_factor_sd_map, "Scale factor for sd map");

                register_parameter("perform-hole-filling", &perform_hole_filling, "Whether to perform hole filling on map");
                register_parameter("max-size-hole", &max_size_hole, "Maximal size for hole");

                register_parameter("std-thres-masking", &std_thres_masking, "Number of noise std for masking");
                register_parameter("mapping-with-masking", &mapping_with_masking, "Whether to compute and apply a mask for mapping");
            };

            std::string skip_processing_meta_field = GADGETRON_SKIP_PROCESSING_AFTER_RECON;

            bool imaging_prep_time_from_protocol = true;

            bool send_map = true;
            bool send_sd_map = true;

            std::string color_lut_map = "GadgetronParametricMap.pal";
            double window_center_map = 4.0;
            double window_width_map = 8.0;

            std::string color_lut_map_3T = "GadgetronParametricMap_3T.pal";
            double window_center_map_3T = 4.0;
            double window_width_map_3T = 8.0;

            double scaling_factor_map = 10.0;

            std::string color_lut_sd_map = "GadgetronParametricSDMap.pal";
            double window_center_sd_map = 4.0;
            double window_width_sd_map = 8.0;
            double scaling_factor_sd_map = 100.0;

            bool perform_hole_filling = true;
            int max_size_hole = 20;

            double std_thres_masking = 3.0;
            bool mapping_with_masking = true;
        };

        CmrParametricMappingGadget(const Core::MrdContext &context, const Parameters& params);

    protected:
        const Parameters params_;

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
