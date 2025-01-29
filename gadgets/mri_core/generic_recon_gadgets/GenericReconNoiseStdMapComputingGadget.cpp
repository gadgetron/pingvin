
#include "GenericReconNoiseStdMapComputingGadget.h"
#include <iomanip>

#include "mri_core_kspace_filter.h"
#include "hoNDArray_reductions.h"
#include "mri_core_def.h"
#include "hoNDArray_utils.h"
#include "mri_core_utility.h"
#include "hoNDArray_elemwise.h"

namespace Gadgetron {

    GenericReconNoiseStdMapComputingGadget::GenericReconNoiseStdMapComputingGadget(const Core::Context& context, const Core::GadgetProperties& properties)
        : BaseClass(context, properties)
    {
        auto& h = context.header;

        if (!h.acquisition_system_information)
        {
            GADGET_THROW("acquisitionSystemInformation not found in header. Bailing out");
        }

        GDEBUG_CONDITION_STREAM(verbose, "Number of encoding spaces: " << num_encoding_spaces_);
    }

    void GenericReconNoiseStdMapComputingGadget::process(Core::InputChannel<mrd::ImageArray>& in, Core::OutputChannel& out)
    {
        GDEBUG_CONDITION_STREAM(verbose, "GenericReconNoiseStdMapComputingGadget::process(...) starts ... ");

        for (auto m1: in) {
            process_called_times_++;

            mrd::ImageArray* recon_res_ = &m1;

            // print out recon info
            if (verbose)
            {
                GDEBUG_STREAM("----> GenericReconNoiseStdMapComputingGadget::process(...) has been called " << process_called_times_ << " times ...");
                std::stringstream os;
                recon_res_->data.print(os);
                GDEBUG_STREAM(os.str());
            }

            std::string dataRole = std::get<std::string>(recon_res_->meta[0][GADGETRON_DATA_ROLE].front());
            if (dataRole != GADGETRON_IMAGE_SNR_MAP)
            {
                continue;
            }

            size_t encoding = (size_t)std::get<long>(recon_res_->meta[0]["encoding"].front());
            GADGET_CHECK_THROW(encoding < num_encoding_spaces_);

            std::stringstream os;
            os << "encoding_" << encoding;
            std::string str = os.str();

            size_t RO = recon_res_->data.get_size(0);
            size_t E1 = recon_res_->data.get_size(1);
            size_t E2 = recon_res_->data.get_size(2);
            size_t CHA = recon_res_->data.get_size(3);
            size_t N = recon_res_->data.get_size(4);
            size_t S = recon_res_->data.get_size(5);
            size_t SLC = recon_res_->data.get_size(6);

            // perform std map computation
            if (N < start_N_for_std_map)
            {
                GWARN_STREAM("GenericReconNoiseStdMapComputingGadget, N < start_N_for_std_map - " << N << " - " << start_N_for_std_map);

                continue;
            }

            // make a copy for results
            mrd::ImageArray cm1 = *recon_res_;

            // pass on the incoming image array
            out.push(std::move(m1));

            if (!debug_folder_full_path_.empty())
            {
                gt_exporter_.export_array_complex(cm1.data, debug_folder_full_path_ + "incoming_SNR_images_" + str);
            }

            // compute std map
            size_t startN = start_N_for_std_map;

            hoNDArray<T> repBuf(RO, E1, E2, CHA, N - startN);
            hoNDArray<real_value_type> repBufMag(RO, E1, E2, CHA, N - startN);
            hoNDArray<real_value_type> repBufMagMean(RO, E1, E2, CHA, 1);
            hoNDArray<real_value_type> diffMag(RO, E1, E2, CHA);
            hoNDArray<real_value_type> stdMap(RO, E1, E2, CHA, 1, S, SLC);
            Gadgetron::clear(stdMap);

            hoNDArray<T>& snrMap = cm1.data;

            size_t n, s, slc;

            for (slc = 0; slc < SLC; slc++)
            {
                for (s = 0; s < S; s++)
                {
                    for (n = startN; n < N; n++)
                    {
                        T* pSNRMap = &(snrMap(0, 0, 0, 0, n, s, slc));
                        memcpy(repBuf.begin() + (n - startN)*RO*E1*E2*CHA, pSNRMap, sizeof(T)*RO*E1*E2*CHA);
                    }

                    Gadgetron::abs(repBuf, repBufMag);

                    // compute mean
                    Gadgetron::sum_over_dimension(repBufMag, repBufMagMean, 4);
                    Gadgetron::scal((real_value_type)(1.0 / repBufMag.get_size(4)), repBufMagMean);

                    hoNDArray<real_value_type> stdMapCurr;
                    stdMapCurr.create(RO, E1, E2, CHA, &(stdMap(0, 0, 0, 0, 0, s, slc)));

                    for (n = startN; n < N; n++)
                    {
                        hoNDArray<real_value_type> snr;
                        snr.create(RO, E1, E2, CHA, &(repBufMag(0, 0, 0, 0, n - startN)));

                        Gadgetron::subtract(snr, repBufMagMean, diffMag);
                        Gadgetron::multiply(diffMag, diffMag, diffMag);

                        Gadgetron::add(stdMapCurr, diffMag, stdMapCurr);
                    }

                    Gadgetron::scal((real_value_type)(1.0 / (N - startN)), stdMapCurr);
                    Gadgetron::sqrt(stdMapCurr, stdMapCurr);
                }
            }

            if (!debug_folder_full_path_.empty())
            {
                gt_exporter_.export_array(stdMap, debug_folder_full_path_ + "std_map_" + str);
            }

            GDEBUG_CONDITION_STREAM(verbose, "GenericReconNoiseStdMapComputingGadget::process(...) ends ... ");

            // update image headers
            cm1.data.clear();
            Gadgetron::real_to_complex(stdMap, cm1.data);

            size_t num = cm1.meta.size();
            for (size_t n = 0; n < num; n++)
            {
                cm1.meta[n][GADGETRON_DATA_ROLE] = {GADGETRON_IMAGE_STD_MAP};

                cm1.meta[n][GADGETRON_IMAGECOMMENT] = {"GT", "SNR", GADGETRON_IMAGE_STD_MAP};

                cm1.meta[n][GADGETRON_SEQUENCEDESCRIPTION] = {"_STD_MAP"};
                cm1.meta[n][GADGETRON_USE_DEDICATED_SCALING_FACTOR] = {(long)1};
            }

            num = cm1.headers.size();
            for (size_t n = 0; n < num; n++)
            {
                auto prev_series_index = cm1.headers(n).image_series_index.value_or(0);
                cm1.headers(n).image_series_index = prev_series_index * 10;
            }

            // ----------------------------------------------------------
            // send out results
            // ----------------------------------------------------------
            out.push(std::move(cm1));
        }
    }

    // ----------------------------------------------------------------------------------------

    GADGETRON_GADGET_EXPORT(GenericReconNoiseStdMapComputingGadget)

}
