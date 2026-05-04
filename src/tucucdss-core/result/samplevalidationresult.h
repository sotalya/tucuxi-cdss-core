// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef SAMPLEVALIDATIONRESULT_H
#define SAMPLEVALIDATIONRESULT_H

#include <string>

#include "tucucore/drugtreatment/sample.h"

#include "tucucdss-core/result/abstractvalidationresult.h"


namespace Tucuxi {
namespace Xpert {

enum class PercentileBucket
{
    LT_P10,
    P10_P25,
    P25_P75,
    P75_P90,
    GT_P90
};

/// \brief This class stores the validation result for a patient sample.
///        It contains a pointer to the sample concerned and the number of
///        the group in which the sample is evaluated.
///
///        The number of the group goes from 1 to 100 which corresponds to the 100
///        separations produced by the 99 percentiles.
///
///        If the group number is 1, the sample is located before the first percentile.
///        If the group number is 2, the sample is located before the second percentile but after the first
///        and so on...
///
///        The 100th group is the one after the 99th percentile.
///
///        The warning message indicates  if the sample is abnormally too low/high in terms
///        of percentile.
///
///        The percentiles described in the comments are the percentiles
///        recovered by an "a priori" percentile computation.
class SampleValidationResult : public AbstractValidationResult<Core::Sample>
{
public:
    /// \brief Constructor.
    /// \param _sample Patient sample concerned by this validation result.
    ///                The sample must have at least the same lifetime as this object.
    /// \param _groupNumberOver99Percentile Position of the group on the 99 percentiles.
    SampleValidationResult(
            const Core::Sample* _sample,
            unsigned _groupNumberOver99Percentile,
            const WarningLevel& _warningLevel,
            bool _isAscending);

    // Getters

    /// \brief Get the warning level of the relevant sample. It
    ///        doesn't take into account whether the warning is set or not.
    /// \return WarningLevel::CRITICAL if the sample is <=5 or >95, otherwise
    ///         WarningLevel::NORMAL.
    ///         This function does not mean that a warning is set.
    [[nodiscard]] WarningLevel getWarningLevel() const override;

    /// \brief Gets the group number of the current sample it is in.
    ///        The group is between 1 and 100. These groups are the groups formed by
    ///        99 percentiles.
    /// \return The number of the group to which the sample belongs.
    [[nodiscard]] unsigned getGroupNumberOver99Percentile() const;

    [[nodiscard]] bool isAscending() const;

    static std::string computePercentileSentence(
            unsigned _percentile,
            double _sampleValue,
            const std::string& _sampleUnit,
            const std::string& _doseText,
            bool _isAscending);

protected:
    /// \brief Compute a warning message based on a group position number.
    ///        It uses the LanguageManager and assumes that the correct language is
    ///        loaded.
    /// \param _groupNumberOver99Percentile The number of the group to which the sample belongs.
    /// \return The warning message for the group position number. May be emptry string.
    static std::string computeWarning(unsigned _groupNumberOver99Percentile);

    static PercentileBucket bucketOf(unsigned _percentile);

    /// \brief Position of the group on the 99 percentiles.
    unsigned m_groupNumberOver99Percentile;

    bool m_isAscending;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // SAMPLEVALIDATIONRESULT_H
