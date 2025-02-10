/** \file   CmrRealTimeLAXCineAIAnalysisGadget.h
    \brief  This is the class gadget for landmark detection of LAX RealTime Cine images.
    \author Hui Xue
*/

#pragma once

#include "generic_recon_gadgets/GenericReconBase.h"
#include "hoMRImage.h"
#include "hoNDImageContainer2D.h"
#include "python_toolbox.h"

namespace Gadgetron {

    class CmrRealTimeLAXCineAIAnalysisGadget : public GenericReconImageArrayBase
    {
    public:
        typedef GenericReconImageArrayBase BaseClass;
        typedef hoNDImageContainer2D < hoMRImage<float, 2> > ImageContainerMagType;

        struct Parameters : public BaseClass::Parameters {
            Parameters(const std::string& prefix): BaseClass::Parameters(prefix, "CmrRealTimeLAXCineAIAnalysisGadget") {
                register_parameter("perform-AI", &perform_AI, "Whether to perform AI");
                register_parameter("model-file", &model_file, "Cine AI ONNX model file for lax landmark detection");
                register_parameter("model-file-sha256", &model_file_sha256, "Checksum sha256 of the model to check its integrity");
                register_parameter("oper-RO", &oper_RO, "Operation image size for AI, RO");
                register_parameter("oper-E1", &oper_E1, "Operation image size for AI, E1");
                register_parameter("pixel-size-send", &pixel_size_send, "Pixel size used for AI and image sending");
                register_parameter("model-url", &model_url, "url to download the model");
                register_parameter("model-dest", &model_dest, "destination folder to install model, under ${GADGETRON_INSTALL_PYTHON_MODULE_PATH}");
            };

            bool perform_AI = true;
            std::string model_file = "CMR_landmark_network_RO_320_E1_320_CH2_CH3_CH4_Myo_Pts_sFOV_LossMultiSoftProb_KLD_Dice_Pytorch_1.8.0a0+37c1f4a_2021-08-08_20210808_085042.onnx";
            std::string model_file_sha256 = "48efe3e70b1ff083c9dd0066469f62bf495e52857d68893296e7375b69f691e4";
            size_t oper_RO = 320;
            size_t oper_E1 = 320;
            double pixel_size_send = 1.0;
            std::string model_url = "https://gadgetrondata.blob.core.windows.net/cmr-ai-models/";
            std::string model_dest = "cmr_lax_landmark_detection";
        };

        CmrRealTimeLAXCineAIAnalysisGadget(const Core::MrdContext &context, const Parameters& params);

    protected:
        const Parameters params_;

        std::string gt_home_;
        std::string gt_model_home_;

        mrd::EncodingCounters meas_max_idx_;

        boost::python::object model_;

        // --------------------------------------------------
        // functional functions
        // --------------------------------------------------

        // default interface function
        void process(Core::InputChannel<mrd::ImageArray>& in, Core::OutputChannel& out) override;

        // function to perform the mapping
        virtual int perform_LAX_detection_AI(mrd::ImageArray& lax, mrd::ImageArray& lax_ai);

        // utility functions
        void convert_array_to_image_container(mrd::ImageArray& lax, hoNDImageContainer2D < hoMRImage<float, 2> >& lax_container);
        void convert_image_container_to_array(hoNDImageContainer2D < hoMRImage<float, 2> >& lax_container, mrd::ImageArray& lax);
        void plot_landmarks_on_images(hoNDImageContainer2D < hoMRImage<float, 2> > & lax_container, const hoNDArray<float>& pts);
        void attach_info_to_report(hoNDImageContainer2D < hoMRImage<float, 2> >& lax_container, const hoNDArray<float>& pts);
    };
}
