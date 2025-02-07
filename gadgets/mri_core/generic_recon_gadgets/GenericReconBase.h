/** \file   GenericReconBase.h
    \brief  This serves an optional base class gadget for the generic chain.
            Some common functionalities are implemented here and can be reused in specific recon gadgets.
            This gadget is instantiated for ReconData and ImageArray
    \author Hui Xue
*/

#pragma once

#include <complex>
#include "Node.h"
#include "GadgetronTimer.h"

#include "mri_core_def.h"
#include "mri_core_utility.h"
#include "mri_core_stream.h"

#include "ImageIOAnalyze.h"

#include "pingvin_sha1.h"

#include "GenericReconStreamDef.h"

#include <boost/filesystem.hpp>

namespace Gadgetron {

    template <typename ...T>
    class GenericReconBase : public Core::MRChannelGadget<T...>
    {
    public:
        typedef Core::MRChannelGadget<T...> BaseClass;

        struct Parameters : public Core::NodeParameters {
            Parameters(const std::string& prefix, const std::string& description) : NodeParameters(prefix, description) {
                register_flag("verbose", &verbose, "Whether to print more information");
                register_flag("perform-timing", &perform_timing, "Whether to perform timing on some computational steps");
                register_parameter("debug-folder", &debug_folder, "If set, the debug output will be written out");
                // register_parameter("time-tick", &time_tick, "Time tick in ms");
            };
            bool verbose = false;
            bool perform_timing = false;
            std::string debug_folder;
            // float time_tick = 2.5;
        };

        GenericReconBase(const Core::MrdContext& context, const Parameters& params)
            : BaseClass(context, params)
            , params_(params)
            , verbose(params_.verbose)
            , num_encoding_spaces_(context.header.encoding.size())
            , process_called_times_(0)
        {
            gt_timer_.set_timing_in_destruction(false);
            gt_timer_local_.set_timing_in_destruction(false);

            if (!params_.debug_folder.empty())
            {
                Gadgetron::get_debug_folder_path(params_.debug_folder, debug_folder_full_path_);
                GDEBUG_CONDITION_STREAM(params_.verbose, "Debug folder is " << debug_folder_full_path_);

                // Create debug folder if necessary
                boost::filesystem::path boost_folder_path(debug_folder_full_path_);
                try
                {
                    boost::filesystem::create_directories(boost_folder_path);
                }
                catch (...)
                {
                    GADGET_THROW("Error creating the debug folder.\n");
                }
            }
            else
            {
                GDEBUG_CONDITION_STREAM(params_.verbose, "Debug folder is not set ... ");
            }

            this->gt_streamer_.verbose_ = this->params_.verbose;
            this->gt_streamer_.initialize_stream_name_buffer(context.env);
        }

        /** TODO: This is a duplicate of this->params_.verbose, for convenience, since it is used
         * by external classes, such as ImageArraySendMixin.
         * 
         * This should be fixed...
         */
        const bool verbose;

    protected:
        const Parameters params_;

        // number of encoding spaces in the protocol
        size_t num_encoding_spaces_;

        // number of times the process function is called
        size_t process_called_times_;

        // --------------------------------------------------
        // variables for debug and timing
        // --------------------------------------------------

        // clock for timing
        Gadgetron::GadgetronTimer gt_timer_local_;
        Gadgetron::GadgetronTimer gt_timer_;

        // debug folder
        std::string debug_folder_full_path_;

        // exporter
        Gadgetron::ImageIOAnalyze gt_exporter_;

        // --------------------------------------------------
        // data stream
        // --------------------------------------------------
        GenericReconMrdStreamer gt_streamer_;

        // --------------------------------------------------
        // gadget functions
        // --------------------------------------------------
        virtual void process(Core::InputChannel<T...>& in, Core::OutputChannel& out) override = 0;
    };

    class GenericReconAcquisitionBase :public GenericReconBase < mrd::AcquisitionHeader >
    {
    public:
        using GenericReconBase < mrd::AcquisitionHeader >::GenericReconBase;
    };

    class GenericReconDataBase :public GenericReconBase < mrd::ReconData >
    {
    public:
        using GenericReconBase < mrd::ReconData >::GenericReconBase;
    };

    class GenericReconImageArrayBase :public GenericReconBase < mrd::ImageArray >
    {
    public:
        using GenericReconBase < mrd::ImageArray >::GenericReconBase;
    };

    template <typename T>
    class GenericReconImageBase :public GenericReconBase < mrd::Image<T> >
    {
    public:
        using GenericReconBase < mrd::Image<T> >::GenericReconBase;
    };
}
