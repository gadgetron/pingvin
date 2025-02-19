#pragma once

#include "Pipeline.h"

#include "mri/noise.h"
#include "mri/grappa.h"
#include "mri/default.h"
#include "mri/epi.h"
#include "mri/cartesian_grappa.h"
#include "mri/cartesian_spirit.h"
#include "mri/grappa_epi.h"
#include "mri/cmr.h"
#include "mri/parallel_bypass.h"
#include "mri/streams.h"
#include "mri/denoise.h"

#include "file_search.h"


namespace Pingvin {

class PipelineRegistry {
public:
    PipelineRegistry() {
        register_pipeline(&file_search);

        register_pipeline(&noise_dependency);
        register_pipeline(&default_mr);
        register_pipeline(&default_mr_optimized);

        register_pipeline(&epi_2d);

        register_pipeline(&grappa);
        register_pipeline(&grappa_cpu);

        register_pipeline(&cartesian_grappa);
        register_pipeline(&cartesian_grappa_snr);
        register_pipeline(&grappa_denoise);

        register_pipeline(&cartesian_spirit);
        register_pipeline(&cartesian_spirit_nonlinear);

        register_pipeline(&grappa_epi);

        register_pipeline(&cmr_cine_binning);
        register_pipeline(&cmr_mapping_t1_sr);
        register_pipeline(&cmr_rtcine_lax_ai);

        register_pipeline(&example_parallel_bypass);

        register_pipeline(&stream_cartesian_grappa_imagearray);
        register_pipeline(&stream_cartesian_grappa);
        register_pipeline(&stream_image_array_scaling);
        register_pipeline(&stream_image_array_split);
        register_pipeline(&stream_complex_to_float);
        register_pipeline(&stream_float_to_fixed_point);
    }

    IPipelineBuilder* get(const std::string& name) const
    {
        if (builders_.count(name)) {
            return builders_.at(name);
        }
        return nullptr;
    }

    auto builders() const
    {
        std::vector<IPipelineBuilder*> result;
        for (const auto& [name, builder] : builders_) {
            result.push_back(builder);
        }
        return result;
    }

protected:
    void register_pipeline(IPipelineBuilder* builder)
    {
        builders_[builder->name()] = builder;
    }

private:
    std::map<std::string, IPipelineBuilder*> builders_;
};

}