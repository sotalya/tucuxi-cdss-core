// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef JUSTIFICATIONCREATOR_H
#define JUSTIFICATIONCREATOR_H

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

enum class Interval
{
    DAILY,
    WEEKLY,
    LASTING
};

struct DosageInfo
{
    double dose;
    Interval interval;
    double timeInterval;
    std::string doseText;
};

///
/// \brief Class responsible for creating justifications based on dosage information.
///
class JustificationCreator : public AbstractXpertFlowStep
{
private:
    Core::DoseValue m_oldDose; ///< Stores the previous dose value.

public:
    ///
    /// \brief Executes the justification creation process.
    /// \param _xpertRequestResult The result object to store the justification.
    ///
    void perform(XpertRequestResult& _xpertRequestResult) override;

private:
    Core::DosageAdjustment getBestAdjustment(const Core::AdjustmentData& _adjustmentData);
    void setJustificationExposure(
            Justification& _justification,
            const XpertRequestResult& _xpertRequestResult,
            const Core::DosageAdjustment& _bestAdj);
    void setJustificationDose(Justification& _justification, double _oldDose, double _newDose);
    void treatJustificationIntervalInfo(
            Justification& _justification, const DosageInfo& _oldDosage, const DosageInfo& _newDosage);

    ///
    /// \brief Extracts dosage information from an abstract dosage object.
    /// \param _dosage The input abstract dosage.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getAbstractDosage(const Core::Dosage& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a dosage loop object.
    /// \param _dosage The input dosage loop object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::DosageLoop& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a steady-state dosage object.
    /// \param _dosage The input steady-state dosage object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::DosageSteadyState& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a repeat dosage object.
    /// \param _dosage The input repeat dosage object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::DosageRepeat& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a dosage sequence object.
    /// \param _dosage The input dosage sequence object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::DosageSequence& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a parallel dosage sequence object.
    /// \param _dosage The input parallel dosage sequence object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::ParallelDosageSequence& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a lasting dose object.
    /// \param _dosage The input lasting dose object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::LastingDose& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a daily dose object.
    /// \param _dosage The input daily dose object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::DailyDose& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Extracts dosage information from a weekly dose object.
    /// \param _dosage The input weekly dose object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getDosage(const Core::WeeklyDose& _dosage, DosageInfo& _dosageInfo) const;

    ///
    /// \brief Combines dosage indications with a prefix.
    /// \param _dosageIndication The main dosage indication.
    /// \param _dosageIndicationChain Additional dosage information to be prefixed.
    /// \return The combined dosage string with the prefix.
    ///
    std::string prefixDosage(const std::string& _dosageIndication, const std::string& _dosageIndicationChain) const;

    ///
    /// \brief Extracts dosage information from a single dose object.
    /// \param _dosage The input single dose object.
    /// \param _dose Output parameter to store the extracted dose value.
    /// \param _doseText Output parameter to store the textual representation of the dose.
    ///
    void getSingleDose(const Core::SingleDose& _dosage, DosageInfo& _dosageInfo) const;

    template<typename T>
    bool tryGetDosage(const Core::Dosage& _dosage, DosageInfo& _dosageInfo) const;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // JUSTIFICATIONCREATOR_H
