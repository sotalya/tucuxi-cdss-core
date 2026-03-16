// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef SAMPLEVALIDATOR_H
#define SAMPLEVALIDATOR_H

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/drugtreatment/sample.h"

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"
#ifndef FRIEND_TEST
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test
// NOLINTEND(cppcoreguidelines-macro-usage)
#endif

namespace Tucuxi {
namespace Xpert {

/// \brief This class evaluates patient samples.
///        Per sample, it makes an "a priori" percentile request for the sample and submits it
///        to the Tucuxi computing core. The query asks for the percentiles 1 - 99, and then locates the
///        position of the sample in the result.
///
///        The group is a number from 1 to 100 that corresponds to the 100 separations produced by the 99 percentiles.
///
///        If the group number is 1, the sample is located before the first percentile.
///        If the group number is 2, the sample is located before the second percentile but after the first.
///        And so on...
///
///        The 100th group is the one that is after the 99th percentile.
///
///        Each sample is evaluated. In the future version, perhaps consider forgetting the too old samples.
///
///        It assumes that the LanguageManager is loaded with a complete translations file.
class SampleValidator : public AbstractXpertFlowStep
{
public:
    /// \brief Evaluate each sample in the treatment from the XpertRequestResult.
    /// \param _xpertRequestResult XpertRequestResult containing samples to evaluate.
    void perform(XpertRequestResult& _xpertRequestResult) override;

protected:
    /// \brief Given a percentiles data (Tucuxi core response) find where the sample is located.
    ///        This methods only takes times[0] and concentrations[0] of the cycleData. Think to
    ///        modify it when the cycleData will implement the analytes feature.
    /// \param _percentilesData Response of the core with 99 percentiles.
    /// \param _sample The patient sample to be positioned.
    /// \return The position of the sample with respect to the percentiles from 1 to 100.
    /// \throw invalid_argument When the conversion of the sample to match the percentile unit
    ///        fails or if the sample date is not linked to any cycleData.
    unsigned findGroupPositionOver99Percentiles(
            const Core::PercentilesData* _percentilesData, const Core::Sample& _sample, bool& _isAscending) const;

    std::vector<std::vector<const Core::Sample*>> groupSamplesByTime(const std::vector<Core::Sample*>& _samples);

    // For testing purposes, the tests works with findGroupPositionOver99Percentiles and not with getSampleValidations. This is easier
    // because it allows us to forge our own percentiles data and to be able to predict the location of some predetermined samples.
    // friend TestSampleValidator;

    FRIEND_TEST(SampleValidatorTest, GivenPercentilesDataAndSamplesGroupPositionOver99PercentilesFinder);
    FRIEND_TEST(SampleValidatorTest, SampleUnitConversionFailureGroupPositionOver99PercentilesFinder);
    FRIEND_TEST(SampleValidatorTest, SampleDateNotFoundInCycleDataGroupPositionOver99PercentilesFinder);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // SAMPLEVALIDATOR_H
