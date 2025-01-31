#include "FloatToFixedPointGadget.h"
#include "mri_core_def.h"
#include "hoNDArray_math.h"


#include <boost/math/constants/constants.hpp>

namespace Gadgetron
{

    template <typename T>
    struct Converter {
        Converter(float min, float max, float offset) : min_(min), max_(max), offset_(offset) {}

        mrd::Image<T> convert(const mrd::Image<float>& image_in) const {
            auto clamp = [&](float val){
                return lround(std::min<float>(std::max<float>(val, min_), max_));
            };
            auto magnitude = [&](auto val){
                return T(clamp(std::abs(val)));
            };

            auto real_value = [&](auto val){
                return T(clamp(float(val) + offset_));
            };

            auto phase = [&](float val){
                return T(clamp((val * offset_ / boost::math::float_constants::pi) + offset_));
            };

            mrd::Image<T> image_out;
            image_out.head = image_in.head;
            image_out.meta = image_in.meta;
            image_out.data.create(image_in.data.dimensions());
            auto& output_data = image_out.data;

            // Now we're ready to transform the image data
            switch (image_in.head.image_type) {
                case mrd::ImageType::kMagnitude: {
                    std::transform(image_in.data.begin(), image_in.data.end(), output_data.begin(), magnitude);
                }
                    break;

                case mrd::ImageType::kReal:
                case mrd::ImageType::kImag: {
                    std::transform(image_in.data.begin(), image_in.data.end(), output_data.begin(), real_value);

                    if (image_out.meta.count(GADGETRON_IMAGE_WINDOWCENTER) && image_out.meta[GADGETRON_IMAGE_WINDOWCENTER].size() > 0) {
                        long windowCenter = std::get<long>(image_out.meta[GADGETRON_IMAGE_WINDOWCENTER].front());
                        image_out.meta[GADGETRON_IMAGE_WINDOWCENTER] = {windowCenter + offset_};
                    }
                }
                    break;

                case mrd::ImageType::kPhase: {
                    std::transform(image_in.data.begin(), image_in.data.end(), output_data.begin(), phase);
                }
                    break;

                default:
                    throw std::runtime_error("Unknown image type in Image");
            }

            return std::move(image_out);
        }

        float min_;
        float max_;
        float offset_;
    };

    template <typename T>
    void convert_image(const Converter<T>& converter, Core::InputChannel<mrd::Image<float>> &input, Core::OutputChannel &output)
    {
        for (auto img: input) {
            output.push(converter.convert(img));
        }
    }

    void FloatToFixedPointGadget::process(Core::InputChannel<mrd::Image<float>> &input, Core::OutputChannel &output)
    {
        if (parameters_.type == "ushort") {
            Converter<unsigned short> converter(parameters_.min_intensity, parameters_.max_intensity, parameters_.intensity_offset);
            convert_image(converter, input, output);
        } else if (parameters_.type == "short") {
            Converter<short> converter(parameters_.min_intensity, parameters_.max_intensity, parameters_.intensity_offset);
            convert_image(converter, input, output);
        } else if (parameters_.type == "uint") {
            Converter<unsigned int> converter(parameters_.min_intensity, parameters_.max_intensity, parameters_.intensity_offset);
            convert_image(converter, input, output);
        } else if (parameters_.type == "int") {
            Converter<int> converter(parameters_.min_intensity, parameters_.max_intensity, parameters_.intensity_offset);
            convert_image(converter, input, output);
        } else {
            throw std::runtime_error("Unsupported type");
        }
    }

    GADGETRON_GADGET_EXPORT(FloatToFixedPointGadget)
}
