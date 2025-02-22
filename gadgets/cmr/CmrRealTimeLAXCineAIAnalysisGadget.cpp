
#include <mutex>
#include "CmrRealTimeLAXCineAIAnalysisGadget.h"
#include "hoNDImage_util.h"
#include "GadgetronTimer.h"
#include "mri_core_def.h"
#include "hoNDBSpline.h"
#include "cmr_time_stamp.h"
#include "hoNDKLT.h"
#include "cmr_image_container_util.h"

static const int GADGET_FAIL = -1;
static const int GADGET_OK = 0;

namespace Gadgetron {

    // a singleton class to load model
    // the current finding is the python runtime may not always release memory after loading in the model and related libraries
    // as a result, repeated model loading (e.g. from multiple calls of this chain) will increase VM usage
    // although not fully resolving the problem, making the model loading a singleton can mitigate this problme

    class CmrRealTimeLAXCineAIModel
    {
    public:

        static CmrRealTimeLAXCineAIModel* instance();
        boost::python::object get_model(const std::string& model_home, const std::string& model_file);

    protected:
        CmrRealTimeLAXCineAIModel() = default;
        static CmrRealTimeLAXCineAIModel* instance_;
        std::map<std::string, boost::python::object> models_;
        std::mutex model_mtx_;

        static void load_from_file(const std::string& model_home, const std::string& model_file, boost::python::object& model);
    };

    CmrRealTimeLAXCineAIModel* CmrRealTimeLAXCineAIModel::instance()
    {
        if (!instance_)
        {
            instance_ = new CmrRealTimeLAXCineAIModel();
        }

        return instance_;
    }

    boost::python::object CmrRealTimeLAXCineAIModel::get_model(const std::string& model_home, const std::string& model_file)
    {
        std::lock_guard<std::mutex> guard(model_mtx_);

        std::string model_key = model_home + "/" + model_file;
        auto model_ptr = models_.find(model_key);
        if(model_ptr==models_.end())
        {
            models_[model_key] = boost::python::object();
            load_from_file(model_home, model_file, models_[model_key]);
        }
        return models_[model_key];
    }

    void CmrRealTimeLAXCineAIModel::load_from_file(const std::string& model_home, const std::string& model_file, boost::python::object& model)
    {
        try
        {
            PythonFunction<boost::python::object> load_model_onnx("gadgetron_cmr_landmark_detection", "load_model_onnx");
            model = load_model_onnx(model_home, model_file);
            bp::incref(model.ptr());
            GDEBUG_STREAM("Model is loaded in CmrRealTimeLAXCineAIModel");
        }
        catch(...)
        {
            GERROR_STREAM("Exceptions happened in CmrRealTimeLAXCineAIModel::load_from_file for " << model_home << " - " << model_file);
            throw;
        }
    }

    CmrRealTimeLAXCineAIModel* CmrRealTimeLAXCineAIModel::instance_ = NULL;

    // ----------------------------------------------------

    CmrRealTimeLAXCineAIAnalysisGadget::CmrRealTimeLAXCineAIAnalysisGadget(const Core::Context& context, const Core::GadgetProperties& properties)
        : BaseClass(context, properties)
    {
        try
        {
            std::string gadgetron_home = context.paths.gadgetron_home.generic_string();
            boost::filesystem::path gadgetron_python_path = context.paths.gadgetron_home / "share" / "gadgetron" / "python";

            Gadgetron::initialize_python();
            Gadgetron::add_python_path(gadgetron_python_path.generic_string());
            this->gt_home_ = gadgetron_python_path.generic_string();

            boost::filesystem::path model_path = context.paths.gadgetron_home / "share" / "gadgetron" / "python" / this->model_dest;
            Gadgetron::add_python_path(model_path.generic_string());
            this->gt_model_home_ = model_path.generic_string();

            GDEBUG_STREAM("Set up python path using context : " << this->gt_home_);
            GDEBUG_STREAM("Set up model path using context : " << this->gt_model_home_);
        }
        catch (...)
        {
            GERROR_STREAM("Exception happened when adding  path to python ... ");
            throw;
        }

        auto& h = context.header;
        if (h.encoding.size() == 0)
        {
            GADGET_THROW("Missing encoding section");
        }

        mrd::EncodingSpaceType e_space = h.encoding[0].encoded_space;
        mrd::EncodingSpaceType r_space = h.encoding[0].recon_space;
        mrd::EncodingLimitsType e_limits = h.encoding[0].encoding_limits;

        meas_max_idx_.kspace_encode_step_1 = (uint16_t)(h.encoding[0].recon_space.matrix_size.y);

        meas_max_idx_.set = (e_limits.set && (e_limits.set->maximum > 0)) ? e_limits.set->maximum : 0;
        meas_max_idx_.phase = (e_limits.phase && (e_limits.phase->maximum > 0)) ? e_limits.phase->maximum : 0;

        meas_max_idx_.kspace_encode_step_2 = (uint16_t)(h.encoding[0].recon_space.matrix_size.z);

        meas_max_idx_.contrast = (e_limits.contrast && (e_limits.contrast->maximum > 0)) ? e_limits.contrast->maximum : 0;

        meas_max_idx_.slice = (e_limits.slice && (e_limits.slice->maximum > 0)) ? e_limits.slice->maximum : 0;

        meas_max_idx_.repetition = e_limits.repetition ? e_limits.repetition->maximum : 0;

        meas_max_idx_.average = e_limits.average ? e_limits.average->maximum : 0;

        meas_max_idx_.segment = 0;

        GDEBUG_STREAM("meas_max_idx_.slice is " << meas_max_idx_.slice.value_or(0));

        // load the model
        if (this->perform_AI)
        {
            std::string model_file_name = this->gt_model_home_ + "/" + this->model_file;
            boost::filesystem::path p(model_file_name);

            if (boost::filesystem::exists(p) && boost::filesystem::is_regular_file(p))
            {
                GDEBUG_STREAM("model file already exists :  " << model_file_name);
            }
            else
            {
                GDEBUG_STREAM("model file does not exist :  " << model_file_name);
            }

            // check the model file, if not exists or invalid, download it
            gt_timer_.start("gadgetron_cmr_landmark_detection_util, check_and_get_model");
            PythonFunction<> check_and_get_model("gadgetron_cmr_landmark_detection_util", "check_and_get_model");
            check_and_get_model(this->model_url, this->model_file, this->gt_model_home_, this->model_file_sha256);
            gt_timer_.stop();

            // load model using the singleton
            gt_timer_.start("CmrRealTimeLAXCineAIModel, load model");
            model_ = CmrRealTimeLAXCineAIModel::instance()->get_model(this->gt_model_home_, this->model_file);
            gt_timer_.stop();
        }
    }

    void CmrRealTimeLAXCineAIAnalysisGadget::process(Core::InputChannel<mrd::ImageArray>& in, Core::OutputChannel& out)
    {
        for (auto m1 : in) {
            if (perform_timing) { gt_timer_local_.start("CmrRealTimeLAXCineAIAnalysisGadget::process"); }

            GDEBUG_CONDITION_STREAM(verbose, "CmrRealTimeLAXCineAIAnalysisGadget::process(...) starts ... ");

            // -------------------------------------------------------------

            process_called_times_++;

            // -------------------------------------------------------------

            mrd::ImageArray& lax = m1;

            // print out data info
            if (verbose)
            {
                GDEBUG_STREAM("----> CmrRealTimeLAXCineAIAnalysisGadget::process(...) has been called " << process_called_times_ << " times ...");
                std::stringstream os;
                lax.data.print(os);
                GDEBUG_STREAM(os.str());
            }

            // -------------------------------------------------------------

            size_t encoding = (size_t)std::get<long>(lax.meta[0]["encoding"].front());
            GADGET_CHECK_THROW(encoding < num_encoding_spaces_);

            size_t RO = lax.data.get_size(0);
            size_t E1 = lax.data.get_size(1);
            size_t E2 = lax.data.get_size(2);
            size_t CHA = lax.data.get_size(3);
            size_t N = lax.data.get_size(4);
            size_t S = lax.data.get_size(5);
            size_t PHS = lax.data.get_size(6);

            std::stringstream os;
            os << "_encoding_" << encoding << "_processing_" << process_called_times_;
            std::string str = os.str();

            // -------------------------------------------------------------

            if (!this->debug_folder_full_path_.empty())
            {
                gt_exporter_.export_array_complex(lax.data, this->debug_folder_full_path_ + "data" + str);
            }

            // -------------------------------------------------------------

            // send out the incoming images first
            out.push(m1);

            // call the AI analysis
            mrd::ImageArray lax_ai;
            int status = this->perform_LAX_detection_AI(lax, lax_ai);

            // send out AI analysis results
            if(status==GADGET_OK)
            {
                out.push(std::move(lax_ai));
            }

            // -------------------------------------------------------------

            if (perform_timing) { gt_timer_local_.stop(); }
        }
    }

    void CmrRealTimeLAXCineAIAnalysisGadget::convert_array_to_image_container(mrd::ImageArray& lax, hoNDImageContainer2D < hoMRImage<float, 2> >& lax_container)
    {
        size_t RO = lax.data.get_size(0);
        size_t E1 = lax.data.get_size(1);
        size_t PHS = lax.meta.size();

        std::vector<size_t> dim2D(2);
        dim2D[0] = RO;
        dim2D[1] = E1;

        std::vector<size_t> cols(1, PHS);
        lax_container.create(cols);

        for (size_t phs = 0; phs < PHS; phs++)
        {
            lax_container(0, phs).create(dim2D);
            lax_container(0, phs).header_ = lax.headers(phs);
            lax_container(0, phs).attrib_ = lax.meta(phs);

            std::complex<float>* pData = lax.data.begin() + phs * RO * E1;

            for (size_t k = 0; k < RO * E1; k++)
            {
                lax_container(0, phs)(k) = std::abs(pData[k]);
            }
        }
    }

    void CmrRealTimeLAXCineAIAnalysisGadget::convert_image_container_to_array(hoNDImageContainer2D < hoMRImage<float, 2> >& lax_container, mrd::ImageArray& lax)
    {
        size_t RO = lax_container(0, 0).get_size(0);
        size_t E1 = lax_container(0, 0).get_size(1);
        std::vector<size_t> cols = lax_container.cols();

        size_t PHS = cols[0];

        lax.data.create(RO, E1, 1, 1, PHS, 1, 1);
        lax.headers.create(PHS, 1, 1);
        lax.meta.create(PHS, 1, 1);

        for (size_t phs=0; phs<PHS; phs++)
        {
            lax.headers(phs, 0, 0) = lax_container(0, phs).header_;
            lax.meta(phs, 0, 0) = lax_container(0, phs).attrib_;

            float* pData = lax_container(0, phs).begin();
            std::complex<float>* pLaxData = &lax.data(0, 0, 0, 0, phs, 0, 0);
            for (size_t k = 0; k < RO * E1; k++)
            {
                pLaxData[k] = pData[k];
            }
        }
    }

    void plot_cross_on_image(hoMRImage<float, 2>& im, float px, float py)
    {
        float max_v;
        size_t max_v_ind;
        Gadgetron::maxAbsolute(im, max_v, max_v_ind);

        size_t RO = im.get_size(0);
        size_t E1 = im.get_size(1);

        int s_ro = (int(py)) - 2;
        if (s_ro < 0) s_ro = 0;

        int e_ro = s_ro + 9;
        if (e_ro >= RO) e_ro = RO - 1;

        int s_e1 = (int(px)) - 2;
        if (s_e1 < 0) s_e1 = 0;

        int e_e1 = s_e1 + 9;
        if (e_e1 >= E1) e_e1 = E1 - 1;

        size_t k;
        for (k=s_ro; k<=e_ro; k++)
        {
            im(k, (s_e1 + e_e1) / 2) = max_v + 1;
        }

        for (k = s_e1; k <= e_e1; k++)
        {
            im((s_ro+e_ro)/2, k) = max_v + 1;
        }
    }

    void CmrRealTimeLAXCineAIAnalysisGadget::plot_landmarks_on_images(hoNDImageContainer2D < hoMRImage<float, 2> > & lax_container, const hoNDArray<float>& pts)
    {
        size_t RO = lax_container(0, 0).get_size(0);
        size_t E1 = lax_container(0, 0).get_size(1);
        std::vector<size_t> cols = lax_container.cols();

        size_t PHS = cols[0];

        GADGET_CHECK_THROW(pts.get_size(2) <= PHS);

        for (size_t phs = 0; phs < pts.get_size(2); phs++)
        {
            if(pts(0, 0, phs) >= 0)
                plot_cross_on_image(lax_container(0, phs), pts(0, 0, phs), pts(0, 1, phs));

            if (pts(1, 0, phs) >= 0)
                plot_cross_on_image(lax_container(0, phs), pts(1, 0, phs), pts(1, 1, phs));

            if (pts(2, 0, phs) >= 0)
                plot_cross_on_image(lax_container(0, phs), pts(2, 0, phs), pts(2, 1, phs));
        }
    }

    void CmrRealTimeLAXCineAIAnalysisGadget::attach_info_to_report(hoNDImageContainer2D < hoMRImage<float, 2> > & lax_container, const hoNDArray<float>& pts)
    {
        size_t RO = lax_container(0, 0).get_size(0);
        size_t E1 = lax_container(0, 0).get_size(1);
        std::vector<size_t> cols = lax_container.cols();

        size_t PHS = cols[0];

        GADGET_CHECK_THROW(pts.get_size(2)<=PHS);

        for (size_t phs = 0; phs < pts.get_size(2); phs++)
        {
            std::vector<double> pt(2);
            pt[0] = pts(0, 0, phs);
            pt[1] = pts(0, 1, phs);
            Gadgetron::set_mrd_meta_values(lax_container(0, phs).attrib_, "Gadgetron_Anterior_PT", pt);

            pt[0] = pts(1, 0, phs);
            pt[1] = pts(1, 1, phs);
            Gadgetron::set_mrd_meta_values(lax_container(0, phs).attrib_, "Gadgetron_Posterior_PT", pt);

            pt[0] = pts(2, 0, phs);
            pt[1] = pts(2, 1, phs);
            Gadgetron::set_mrd_meta_values(lax_container(0, phs).attrib_, "Gadgetron_Apical_PT", pt);
        }
    }

    int CmrRealTimeLAXCineAIAnalysisGadget::perform_LAX_detection_AI(mrd::ImageArray& lax, mrd::ImageArray& lax_ai)
    {
        try
        {
            Gadgetron::GadgetronTimer gt_timer(false);

            if (!this->debug_folder_full_path_.empty())
            {
                gt_exporter_.export_array_complex(lax.data, this->debug_folder_full_path_ + "CmrRealTimeLAXCineAIAnalysisGadget_LAX");
            }

            size_t RO = lax.data.get_size(0);
            size_t E1 = lax.data.get_size(1);
            size_t PHS = lax.meta.size();

            ImageContainerMagType lax_container;

            std::vector<size_t> dim2D(2);
            dim2D[0] = RO;
            dim2D[1] = E1;

            this->convert_array_to_image_container(lax, lax_container);

            if (!this->debug_folder_full_path_.empty())
            {
                hoNDArray<float> output;
                Gadgetron::convert_container_to_4D_array(lax_container, output);
                output.squeeze();

                std::stringstream str;
                str << "lax_container";
                gt_exporter_.export_array(output, this->debug_folder_full_path_ + "/" + str.str());
            }

            // -------------------------------------------
            // resample to 1mm2
            ImageContainerMagType lax_highres;
            lax_highres.copyFrom(lax_container);

            double FOV_RO = lax_container(0, 0).header_.field_of_view[0];
            double FOV_E1 = lax_container(0, 0).header_.field_of_view[1];

            double pixel_size = this->pixel_size_send;

            size_t new_RO = (size_t)(FOV_RO / pixel_size + 0.5);
            size_t new_E1 = (size_t)(FOV_E1 / pixel_size + 0.5);

            if (new_RO % 2 == 1) new_RO++;
            if (new_E1 % 2 == 1) new_E1++;

            std::vector<size_t> dim2D_out(2);
            dim2D_out[0] = new_RO;
            dim2D_out[1] = new_E1;

            GDEBUG_STREAM("RT CINE LAX, new image size " << " [ " << new_RO << " " << new_E1 << "]");

            Gadgetron::resample_image_container(lax_container, dim2D_out, lax_highres);

            if (!this->debug_folder_full_path_.empty())
            {
                hoNDArray<float> output;
                Gadgetron::convert_container_to_4D_array(lax_highres, output);
                output.squeeze();

                std::stringstream str;
                str << "lax_highres";
                gt_exporter_.export_array(output, this->debug_folder_full_path_ + "/" + str.str());
            }

            // -------------------------------------------
            // convert to dicom orientation

            ImageContainerMagType lax_highres_dicom;
            lax_highres_dicom.copyFrom(lax_highres);

            if (!this->debug_folder_full_path_.empty())
            {
                hoNDArray<float> output;
                Gadgetron::convert_container_to_4D_array(lax_highres_dicom, output);
                output.squeeze();

                std::stringstream str;
                str << "lax_highres_dicom";
                gt_exporter_.export_array(output, this->debug_folder_full_path_ + "/" + str.str());
            }

            RO = lax_highres_dicom(0, 0).get_size(0);
            E1 = lax_highres_dicom(0, 0).get_size(1);

            // -------------------------------------------

            size_t PHS_detected = PHS;

            hoNDArray<float> lax_images;
            lax_images.create(RO, E1, PHS_detected);
            for (size_t phs = 0; phs < PHS_detected; phs++)
            {
                memcpy(&lax_images(0, 0, phs), lax_highres_dicom(0, phs).begin(), lax_highres_dicom(0, phs).get_number_of_bytes());
            }

            if (!this->debug_folder_full_path_.empty())
            {
                std::stringstream str;
                str << "lax_images_for_AI";
                gt_exporter_.export_array(lax_images, this->debug_folder_full_path_ + "/" + str.str());
            }

            std::stringstream imgs_stream;
            lax_images.print(imgs_stream);
            GINFO(imgs_stream.str().c_str());

            // ---------------------------------------------------------
            // call cmr landmark detection
            hoNDArray<float> pts, probs;

            if(!this->gt_home_.empty())
            {
                GDEBUG_STREAM("=============================================");

                gt_timer_.start("gadgetron_cmr_landmark_detection_util, perform_cmr_landmark_detection");
                PythonFunction< hoNDArray<float>, hoNDArray<float> > perform_cmr_landmark_detection("gadgetron_cmr_landmark_detection", "perform_cmr_landmark_detection");
                float p_thresh=0.1;
                std::tie(pts, probs) = perform_cmr_landmark_detection(lax_images, this->model_, p_thresh, this->oper_RO, this->oper_E1);
                gt_timer_.stop();

                std::stringstream pts_stream;
                pts.print(pts_stream);


                GDEBUG_STREAM("=============================================");

                if (!this->debug_folder_full_path_.empty())
                {
                    std::stringstream str;
                    str << "lax_images_landmark_pts";
                    gt_exporter_.export_array(pts, this->debug_folder_full_path_ + "/" + str.str());
                }

                // -------------------------------------------

                GDEBUG_STREAM("attach landmarks to images ...");
                this->plot_landmarks_on_images(lax_highres_dicom, pts);

                if (!this->debug_folder_full_path_.empty())
                {
                    hoNDArray<float> output;
                    Gadgetron::convert_container_to_4D_array(lax_highres_dicom, output);
                    output.squeeze();

                    std::stringstream str;
                    str << "lax_highres_dicom_with_pts";
                    gt_exporter_.export_array(output, this->debug_folder_full_path_ + "/" + str.str());
                }

                for (size_t phs = 0; phs < PHS; phs++)
                {
                    lax_highres_dicom(0, phs).header_.image_series_index = lax_highres_dicom(0, phs).header_.image_series_index.value_or(1) * 100;

                    lax_highres_dicom(0, phs).attrib_[GADGETRON_IMAGEPROCESSINGHISTORY].push_back(GADGETRON_AI);
                    lax_highres_dicom(0, phs).attrib_[GADGETRON_SEQUENCEDESCRIPTION].push_back(GADGETRON_AI);

                    Gadgetron::set_meta_from_mrd_header(lax_highres_dicom(0, phs).header_, lax_highres_dicom(0, phs).attrib_);
                }

                GDEBUG_STREAM("attach landmarks to attributes ...");
                this->attach_info_to_report(lax_highres_dicom, pts);
            }

            GDEBUG_STREAM("prepare outputs ...");
            this->convert_image_container_to_array(lax_highres_dicom, lax_ai);

            if (!this->debug_folder_full_path_.empty())
            {
                std::stringstream str;
                str << "lax_ai";
                gt_exporter_.export_array_complex(lax_ai.data, this->debug_folder_full_path_ + "/" + str.str());
            }
        }
        catch (...)
        {
            GERROR_STREAM("Exceptions happened in CmrRealTimeLAXCineAIAnalysisGadget::perform_LAX_detection_AI(...) ... ");
            return GADGET_FAIL;
        }

        return GADGET_OK;
    }

    // ----------------------------------------------------------------------------------------

    GADGETRON_GADGET_EXPORT(CmrRealTimeLAXCineAIAnalysisGadget)
}
