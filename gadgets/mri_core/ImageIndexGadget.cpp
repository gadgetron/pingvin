
#include "ImageIndexGadget.h"

#include "Node.h"

#include "log.h"


namespace {

    template<class T, class F>
    mrd::Image<T> update_image_index(mrd::Image<T> image, F &index) {
        image.head.image_index = index(image.head.image_series_index.value_or(0));
        return image;
    }
}

namespace Gadgetron {

    void ImageIndexGadget::process(Core::InputChannel<mrd::AnyImage> &input, Core::OutputChannel &output) {

        std::map<uint16_t, uint16_t> indices{};

        auto index = [&](auto series) { return indices.count(series) ? indices.at(series) : 1; };
        auto increment = [&](auto series) { indices[series] = index(series) + 1; };
        auto increment_and_return = [&](auto series) {
            auto idx = index(series); increment(series);
            GDEBUG_STREAM("Generated image index " << idx << " for image series " << series);
            return idx;
        };

        for (auto image : input) {
            visit([&](auto image) {
                output.push(update_image_index(image, increment_and_return));
            }, image);
        }
    }

}