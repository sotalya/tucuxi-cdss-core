// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iomanip>

#include <spdlog/fmt/fmt.h>

#include "samplevalidationresult.h"

#include "tucucdss-core/language/languagemanager.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

SampleValidationResult::SampleValidationResult(
        const Core::Sample* _sample,
        unsigned _groupNumberOver99Percentile,
        const WarningLevel& _warningLevel,
        bool _isAscending)
    : AbstractValidationResult<Core::Sample>(_sample, computeWarning(_groupNumberOver99Percentile), _warningLevel),
      m_groupNumberOver99Percentile(_groupNumberOver99Percentile), m_isAscending(_isAscending)
{
}

WarningLevel SampleValidationResult::getWarningLevel() const
{
    auto b = bucketOf(m_groupNumberOver99Percentile);
    switch (b) {
    case PercentileBucket::LT_P10:
    case PercentileBucket::GT_P90:
        return WarningLevel::CRITICAL;
    case PercentileBucket::P10_P25:
    case PercentileBucket::P75_P90:
        return WarningLevel::WARNING;
    case PercentileBucket::P25_P75:
    default:
        return WarningLevel::NORMAL;
    }
}

unsigned SampleValidationResult::getGroupNumberOver99Percentile() const
{
    return m_groupNumberOver99Percentile;
}

bool SampleValidationResult::isAscending() const
{
    return m_isAscending;
}

std::string SampleValidationResult::computePercentileSentence(
        unsigned percentile,
        double sampleValue,
        const std::string& sampleUnit,
        const std::string& doseText,
        bool isAscending)
{

    LanguageManager& langMgr = LanguageManager::getInstance();
    std::string prefixMeasured = langMgr.translate("txt_measured_of_prefix");
    std::string prefixDose = langMgr.translate("txt_with_dose_prefix");

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << sampleValue;
    std::string valueStr = oss.str() + " " + sampleUnit;

    std::string body;
    std::string optSuffix;

    switch (bucketOf(percentile)) {
    case PercentileBucket::LT_P10:
        body = langMgr.translate("txt_below_lt_p10");
        optSuffix = langMgr.translate("txt_suffix_lt_p10");
        break;
    case PercentileBucket::P10_P25:
        body = langMgr.translate("txt_below_p10_p25");
        break;
    case PercentileBucket::P25_P75:
        body = langMgr.translate("txt_expected_p25_p75");
        break;
    case PercentileBucket::P75_P90:
        body = langMgr.translate("txt_above_p75_p90");
        break;
    case PercentileBucket::GT_P90:
        body = langMgr.translate("txt_above_gt_p90");
        optSuffix = langMgr.translate("txt_suffix_gt_p90");
        break;
    }
    std::string sentence = fmt::format("{} {} ", prefixMeasured, valueStr);

    if (isAscending) {
        sentence += fmt::format(langMgr.translate("txt_sample_in_ascending"), fmt::arg("hours", "N"));
    }

    sentence += fmt::format("{} {} {}", body, prefixDose, doseText);

    if (!optSuffix.empty()) {
        sentence += optSuffix;
    }

    return sentence;
}

std::string SampleValidationResult::computeWarning(unsigned _groupNumberOver99Percentile)
{
    // If the percentile is within the warning limits.
    if (_groupNumberOver99Percentile <= 10 || _groupNumberOver99Percentile > 90) {

        LanguageManager& langMgr = LanguageManager::getInstance();

        // Get the base of the message and the perentage of the population that is below or above the patient.
        std::string baseWarning = _groupNumberOver99Percentile <= 50 ? langMgr.translate("population_above")
                                                                     : langMgr.translate("population_below");

        unsigned populationPercentage = _groupNumberOver99Percentile <= 50 ? 100 - _groupNumberOver99Percentile
                                                                           : _groupNumberOver99Percentile - 1;

        return std::to_string(populationPercentage) + baseWarning;
    }

    return "";
}

PercentileBucket SampleValidationResult::bucketOf(unsigned p)
{
    if (p <= 10) {
        return PercentileBucket::LT_P10;
    }
    if (p <= 25) {
        return PercentileBucket::P10_P25;
    }
    if (p <= 75) {
        return PercentileBucket::P25_P75;
    }
    if (p <= 90) {
        return PercentileBucket::P75_P90;
    }
    return PercentileBucket::GT_P90;
}

} // namespace Xpert
} // namespace Tucuxi
