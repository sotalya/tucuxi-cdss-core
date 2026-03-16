// SPDX-License-Identifier: AGPL-3.0-or-later
#include <memory.h>

#include "timeafterdoseexporter.h"

#include "tucucore/timeafterdosecalculator.h"

namespace Tucuxi {
namespace Xpert {

void TimeAfterDoseExporter::perform(XpertRequestResult& _xpertRequestResult)
{
    Tucuxi::Core::TimeAfterDoseCalculator tadCalc;

    auto dh = _xpertRequestResult.getTreatment()->getDosageHistory();

    if (dh.getDosageTimeRanges().empty()) {
        return;
    }

    Tucuxi::Core::Samples samples;

    for (const auto& sampleValidationResult : _xpertRequestResult.getSampleValidationResults()) {
        samples.push_back(std::make_unique<Tucuxi::Core::Sample>(*sampleValidationResult.getSource()));
    }

    _xpertRequestResult.setTimeAfterDoseDurations(tadCalc.calculateDurations(samples, dh));
}

} // namespace Xpert
} // namespace Tucuxi
