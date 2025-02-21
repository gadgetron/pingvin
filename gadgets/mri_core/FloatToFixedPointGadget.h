#pragma once

#include "MRNode.h"

namespace Gadgetron
{

    /**
    * This Gadget converts float values to fixed point integer format.
    *
    * How the conversion is done will depend on the image type:
    * Magnitude images: Values above 4095 will be clamped.
    * Real or Imag: Values below -2048 and above 2047 will be clamped. Zero will be 2048.
    * Phase: -pi will be 0, +pi will be 4095.
    *
    */

    class FloatToFixedPointGadget: public Core::MRChannelGadget<mrd::Image<float>>
    {
    public:
        struct Parameters : public Core::NodeParameters {
            Parameters(const std::string& prefix) : Core::NodeParameters(prefix, "Float to Fixed Point Conversion")
            {
                register_parameter("type", &type, "Output integer type (ushort, short, uint, int)");
                register_parameter("max-intensity", &max_intensity, "Maximum intensity value");
                register_parameter("min-intensity", &min_intensity, "Minimum intensity value");
                register_parameter("intensity-offset", &intensity_offset, "Intensity offset");
            }

            std::string type = "ushort";
            float max_intensity = 4095;
            float min_intensity = 0;
            float intensity_offset = 2048;
        };

        FloatToFixedPointGadget(const Core::MRContext& context, const Parameters& params)
            : MRChannelGadget(context, params)
            , parameters_(params)
        {}

        ~FloatToFixedPointGadget() override = default ;

        void process(Core::InputChannel<mrd::Image<float>>& input, Core::OutputChannel& output) override;

    protected:
        Parameters parameters_;
    };
}