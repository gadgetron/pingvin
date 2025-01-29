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
    class GenericReconBase : public Core::ChannelGadget<T...>
    {
    public:
        typedef Core::ChannelGadget<T...> BaseClass;

        GenericReconBase(const Core::Context& context, const Core::GadgetProperties& properties)
            : BaseClass(context, properties)
            , num_encoding_spaces_(context.header.encoding.size()), process_called_times_(0)
        {
            gt_timer_.set_timing_in_destruction(false);
            gt_timer_local_.set_timing_in_destruction(false);

            if (!debug_folder.empty())
            {
                Gadgetron::get_debug_folder_path(debug_folder, debug_folder_full_path_);
                GDEBUG_CONDITION_STREAM(verbose, "Debug folder is " << debug_folder_full_path_);

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
                GDEBUG_CONDITION_STREAM(verbose, "Debug folder is not set ... ");
            }

            // find the buffer names if they are set
            this->gt_streamer_.initialize_stream_name_buffer(context.parameters);
            this->gt_streamer_.verbose_ = this->verbose;
        }

        /// ------------------------------------------------------------------------------------
        /// debug and timing
        NODE_PROPERTY(verbose, bool, "Whether to print more information", false);
        NODE_PROPERTY(debug_folder, std::string, "If set, the debug output will be written out", "");
        NODE_PROPERTY(perform_timing, bool, "Whether to perform timing on some computational steps", false);

        /// ms for every time tick
        NODE_PROPERTY(time_tick, float, "Time tick in ms", 2.5);

    protected:

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
