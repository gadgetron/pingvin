
#include "GenericReconImageArrayScalingGadget.h"
#include <iomanip>

#include "mri_core_kspace_filter.h"
#include "hoNDArray_reductions.h"
#include "mri_core_def.h"
#include "mri_core_coil_map_estimation.h"

#define GENERICRECON_DEFAULT_INTENSITY_SCALING_FACTOR 4.0f
#define GENERICRECON_DEFAULT_INTENSITY_MAX 2048

static const int GADGET_FAIL = -1;
static const int GADGET_OK = 0;
namespace Gadgetron {

    GenericReconImageArrayScalingGadget::GenericReconImageArrayScalingGadget(const Core::Context &context, const Core::GadgetProperties &properties)
        : BaseClass(context, properties)
    {
        auto& h = context.header;

        GDEBUG_CONDITION_STREAM(verbose, "Number of encoding spaces: " << num_encoding_spaces_);

        // resize the scaling_factor_ vector and set to negative
        scaling_factor_.resize(num_encoding_spaces_, -1);

        if (use_constant_scalingFactor)
        {
            float v = scalingFactor;

            if (v > 0)
            {
                for (size_t e = 0; e < num_encoding_spaces_; e++)
                    scaling_factor_[e] = scalingFactor;
            }
            else
            {
                use_constant_scalingFactor = false;
            }
        }
    }

    void GenericReconImageArrayScalingGadget::process(Core::InputChannel< mrd::ImageArray >& in, Core::OutputChannel& out)
    {
        for (auto m1: in) {
            if (perform_timing) { gt_timer_.start("GenericReconImageArrayScalingGadget::process"); }

            GDEBUG_CONDITION_STREAM(verbose, "GenericReconImageArrayScalingGadget::process(...) starts ... ");

            process_called_times_++;

            mrd::ImageArray* recon_res_ = &m1;

            size_t encoding = (size_t)std::get<long>(recon_res_->meta[0]["encoding"].front());
            GADGET_CHECK_THROW(encoding < num_encoding_spaces_);

            std::string dataRole = std::get<std::string>(recon_res_->meta[0][GADGETRON_DATA_ROLE].front());

            std::string imageInfo;

            // ----------------------------------------------------
            // apply scaling
            // ----------------------------------------------------
            double scale_factor = 1.0;

            // snr map
            if (dataRole == GADGETRON_IMAGE_SNR_MAP)
            {
                scale_factor = this->scalingFactor_snr_map;
                Gadgetron::scal((real_value_type)(scale_factor), recon_res_->data);

                std::ostringstream ostr_image;
                ostr_image << "x" << std::setprecision(4) << this->scalingFactor_snr_map;
                imageInfo = ostr_image.str();
            }
            // gfactor
            else if (dataRole == GADGETRON_IMAGE_GFACTOR)
            {
                scale_factor = this->scalingFactor_gfactor_map;
                Gadgetron::scal((real_value_type)(scale_factor), recon_res_->data);

                std::ostringstream ostr_image;
                ostr_image << "x" << std::setprecision(4) << this->scalingFactor_gfactor_map;
                imageInfo = ostr_image.str();
            }
            // snr std map
            else if (dataRole == GADGETRON_IMAGE_STD_MAP)
            {
                scale_factor = this->scalingFactor_snr_std_map;
                Gadgetron::scal((real_value_type)(scale_factor), recon_res_->data);

                std::ostringstream ostr_image;
                ostr_image << "x" << std::setprecision(4) << this->scalingFactor_snr_std_map;
                imageInfo = ostr_image.str();
            }
            // if the config file asks to use the specified scaling factor
            else if (recon_res_->meta[0].count(use_dedicated_scalingFactor_meta_field)
                    && recon_res_->meta[0][use_dedicated_scalingFactor_meta_field].size() > 0)
            {
                scale_factor = this->scalingFactor_dedicated;
                Gadgetron::scal((real_value_type)(scale_factor), recon_res_->data);

                std::ostringstream ostr_image;
                ostr_image << "x" << std::setprecision(4) << this->scalingFactor_dedicated;
                imageInfo = ostr_image.str();
            }
            else
            {
                // compute scaling factor from image and apply it
                if (perform_timing) { gt_timer_.start("compute_and_apply_scaling_factor"); }
                this->compute_and_apply_scaling_factor(*recon_res_, encoding);
                if (perform_timing) { gt_timer_.stop(); }

                scale_factor = this->scaling_factor_[encoding];

                std::ostringstream ostr_image;
                ostr_image << "x" << std::setprecision(4) << this->scaling_factor_[encoding];
                imageInfo = ostr_image.str();
            }

            // ----------------------------------------------------
            // update the image comment
            // ----------------------------------------------------
            size_t num = recon_res_->meta.size();
            for (size_t n = 0; n < num; n++)
            {
                recon_res_->meta[n][GADGETRON_IMAGECOMMENT].push_back(imageInfo);
                recon_res_->meta[n][GADGETRON_IMAGE_SCALE_RATIO] = {scale_factor};
            }

            GDEBUG_CONDITION_STREAM(verbose, "GenericReconImageArrayScalingGadget::process(...) ends ... ");

            out.push(std::move(m1));

            if (perform_timing) { gt_timer_.stop(); }
        }
    }

    int GenericReconImageArrayScalingGadget::compute_and_apply_scaling_factor(mrd::ImageArray& res, size_t encoding)
    {
        // if the scaling factor for this encoding space has not been set yet (it was initialized negative),
        //   compute it.  If it has already been set (therefore, it will be positive), only compute again if
        //   the auto-scaling factor is to be computed for every incoming image array
        if ((scaling_factor_[encoding]<0 || !auto_scaling_only_once) && !use_constant_scalingFactor)
        {
            hoNDArray<real_value_type> mag;
            GADGET_CHECK_EXCEPTION_RETURN(Gadgetron::abs(res.data, mag), GADGET_FAIL);

            // perform the scaling to [0 max_inten_value_]
            size_t ind;
            float maxInten;

            size_t RO = mag.get_size(0);
            size_t E1 = mag.get_size(1);
            size_t E2 = mag.get_size(2);
            size_t num = mag.get_number_of_elements()/(RO*E1*E2);

            if ( num <= 24 )
            {
                GADGET_CHECK_EXCEPTION_RETURN(Gadgetron::maxAbsolute(mag, maxInten, ind), GADGET_FAIL);
            }
            else
            {
                // grab the middle 24 slices/volumes/etc.
                hoNDArray<float> magPartial(RO, E1, E2, 24, mag.get_data_ptr()+(num/2 - 12)*RO*E1*E2);
                GADGET_CHECK_EXCEPTION_RETURN(Gadgetron::maxAbsolute(magPartial, maxInten, ind), GADGET_FAIL);
            }
            if ( maxInten < FLT_EPSILON ) maxInten = 1.0f;

            // if the maximum image intensity is too small or too large
            if ( (maxInten<min_intensity_value) || (maxInten>max_intensity_value) )
            {
                GDEBUG_CONDITION_STREAM(verbose, "Using the dynamic intensity scaling factor - may not have noise prewhitening performed ... ");
                // scale the image (so that the maximum image intensity is the default one)
                scaling_factor_[encoding] = (float)(GENERICRECON_DEFAULT_INTENSITY_MAX) / maxInten;
            }
            // if the maximum image intensity is within limits
            else
            {
                GDEBUG_CONDITION_STREAM(verbose, "Using the fixed intensity scaling factor - must have noise prewhitening performed ... ");
                // starting with the fixed intensity scaling factor, check if the image will
                //   be clipped and, if so, try halving it (up to a minimum)
                scaling_factor_[encoding] = GENERICRECON_DEFAULT_INTENSITY_SCALING_FACTOR;
                while ((maxInten*scaling_factor_[encoding] > max_intensity_value) && (scaling_factor_[encoding] >= 2))
                {
                    scaling_factor_[encoding] /= 2;
                }

                // if even at the minimum we are still clipping, issue a warning and
                //    calculate a scaling factor to cover the whole dynamic range
                if (maxInten*scaling_factor_[encoding] > max_intensity_value)
                {
                    GDEBUG_CONDITION_STREAM(verbose, "The fixed intensity scaling factor leads to dynamic range overflow - switch to dynamic intensity scaling ... ");
                    scaling_factor_[encoding] = (float)(max_intensity_value) / maxInten;
                }
            }

            GDEBUG_CONDITION_STREAM(verbose, "Encoding space - " << encoding << ", scaling_factor_ : " << scaling_factor_[encoding]);
        }
        else
        {
            GDEBUG_CONDITION_STREAM(verbose, "Encoding space - " << encoding << ", using the fixed intensity scaling factor - scaling factor has been preset to be : " << scaling_factor_[encoding] << " ... ");
        }

        // Apply scaling factor
        GADGET_CHECK_EXCEPTION_RETURN(Gadgetron::scal((real_value_type)scaling_factor_[encoding], res.data), GADGET_FAIL);

        return GADGET_OK;
    }



    // ----------------------------------------------------------------------------------------

    GADGETRON_GADGET_EXPORT(GenericReconImageArrayScalingGadget)

}
