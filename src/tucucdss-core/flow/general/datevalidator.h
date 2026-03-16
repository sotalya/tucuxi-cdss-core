// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief Check the plausibility of the dates of doses and samples.
///
///        It assumes that the LanguageManager is loaded with a complete
///        translations file.
class DateValidator : public AbstractXpertFlowStep
{
public:
    /// \brief Evaluate all the dates in the treatment of the given
    ///        XpertRequestResult.
    ///        If the evaluation fails (for example: meaningless dates), the
    ///        XpertRequestResult is invalidated (i.e. it gets an error).
    /// \param _xpertRequestResult XpertRequestResult to evaluate doses.
    void perform(XpertRequestResult& _xpertRequestResult) override;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _dosageHistory Dosage history.
    std::vector<Common::DateTime> extractDoseDates(Core::DosageHistory const& _dosageHistory) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _dosage Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::Dosage& _dosage, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _dosageLoop Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::DosageLoop& _dosageLoop, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _dosageRepeat Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::DosageRepeat& _dosageRepeat, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _dosageSequence Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::DosageSequence& _dosageSequence, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _parallelDosageSequence Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::ParallelDosageSequence& _parallelDosageSequence, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _dosageBoundedList Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDatesDosageBoundedList(
            const Core::DosageBoundedList& _dosageBoundedList, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _singleDoseAtTimeList Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::SingleDoseAtTimeList& _singleDoseAtTimeList, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _simpleDoseList Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::SimpleDoseList& _simpleDoseList, Core::DosageTimeRange const& _timeRange) const;

    /// \brief Helper function to retrieve the administration dates.
    /// \param _singleDose Dosage of interest.
    /// \param _timeRange Time range from which to retrive the dosage dates.
    std::vector<Common::DateTime> extractDoseDates(
            const Core::SingleDose& _singleDose, Core::DosageTimeRange const& _timeRange) const;

private:
    /// \brief Check whether the maximum distance between a dose and the closest
    ///        (future) measure is smaller than a given threshold.
    /// \param _doseDates Dose dates.
    /// \param _measureDates Measurement dates.
    /// \param _threshold Maximum distance accepted.
    /// \returns True if the distance is below the threshold, false otherwise.
    bool checkDoseToMeasureDistances(
            const std::vector<Common::DateTime>& _doseDates,
            const std::vector<Common::DateTime>& _measureDates,
            const Common::Duration& _threshold);

    /// \brief Get a pointer to the sample that has a given date.
    /// \param _samples Vector of samples.
    /// \param _date Date of interest.
    Core::Sample* findSampleByDate(
            const std::vector<std::unique_ptr<Core::Sample>>& _samples, const Common::DateTime& _date);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // DATEVALIDATOR_H
