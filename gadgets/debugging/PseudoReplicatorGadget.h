#pragma once

#include "MRNode.h"

namespace Gadgetron {

class PseudoReplicatorGadget : public Core::MRChannelGadget<mrd::ReconData>
{
public:
	struct Parameters : public Core::NodeParameters {
		Parameters(const std::string& prefix) : Core::NodeParameters(prefix, "Pseudo Replicator") {
			register_parameter("repetitions", &repetitions, "Number of pseudoreplicas to produce");
		}

		int repetitions = 10;
	};

	PseudoReplicatorGadget(const Core::MRContext& context, const Parameters& params)
		: MRChannelGadget(context, params)
		, params_(params)
	{ }

protected:
	const Parameters params_;

	void process(Core::InputChannel<mrd::ReconData>& input, Core::OutputChannel& output) override;
};

} /* namespace Gadgetron */
