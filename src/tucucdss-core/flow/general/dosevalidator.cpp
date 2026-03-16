// SPDX-License-Identifier: AGPL-3.0-or-later
#include <memory>

#include "dosevalidator.h"

#include "tucucommon/unit.h"

#include "tucucore/drugmodel/formulationandroute.h"

#include "tucucdss-core/language/languagemanager.h"
#include "tucucdss-core/result/dosevalidationresult.h"
#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

void DoseValidator::perform(XpertRequestResult& _xpertRequestResult)
{
    // Check if there is a treatment.
    if (_xpertRequestResult.getTreatment() == nullptr) {
        _xpertRequestResult.setErrorMessage("No treatment set.");
        return;
    }

    // Check if there is a drug model.
    if (_xpertRequestResult.getDrugModel() == nullptr) {
        _xpertRequestResult.setErrorMessage("No drug model set.");
        return;
    }

    const Core::DosageHistory& dosageHistory = _xpertRequestResult.getTreatment()->getDosageHistory();
    const Core::FormulationAndRoutes& modelFormulationAndRoutes =
            _xpertRequestResult.getDrugModel()->getFormulationAndRoutes();

    // Explore the dosage history to validate doses.
    try {
        map<const Core::DosageBounded*, DoseValidationResult> results;
        checkDoses(dosageHistory, modelFormulationAndRoutes, results);
        _xpertRequestResult.setDoseResults(std::move(results));
    }
    catch (invalid_argument& e) {
        _xpertRequestResult.setErrorMessage("Patient dosage error found, details: " + std::string(e.what()));
    }
}

void DoseValidator::checkDoses(
        const Core::DosageHistory& _dosageHistory,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    // For each dosage time range.
    for (const unique_ptr<Core::DosageTimeRange>& timeRange : _dosageHistory.getDosageTimeRanges()) {
        checkDoses(*timeRange, _modelFormulationsAndRoutes, _results);
    }
}

void DoseValidator::checkDoses(
        const Core::DosageTimeRange& _timeRange,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    checkDoses(*_timeRange.getDosage(), _modelFormulationsAndRoutes, _results);
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TRY_CHECK(Type)                                                      \
    if (const auto* p = dynamic_cast<const Tucuxi::Core::Type*>(&_dosage)) { \
        checkDoses(*p, _modelFormulationsAndRoutes, _results);               \
    }

void DoseValidator::checkDoses(
        const Core::Dosage& _dosage,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    // The calls order is important here.
    // First start with the subclasses, else it won't work
    TRY_CHECK(SingleDose);
    TRY_CHECK(SimpleDoseList);
    TRY_CHECK(SingleDoseAtTimeList);
    TRY_CHECK(ParallelDosageSequence);
    TRY_CHECK(DosageSequence);
    TRY_CHECK(DosageRepeat);
    TRY_CHECK(DosageLoop);
}

void DoseValidator::checkDoses(
        const Core::DosageLoop& _dosageLoop,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    checkDoses(*_dosageLoop.getDosage(), _modelFormulationsAndRoutes, _results);
}

void DoseValidator::checkDoses(
        const Core::DosageRepeat& _dosageRepeat,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    checkDoses(*_dosageRepeat.getDosage(), _modelFormulationsAndRoutes, _results);
}

void DoseValidator::checkDoses(
        const Core::DosageSequence& _dosageSequence,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    checkDosageBoundedList(_dosageSequence.getDosageList(), _modelFormulationsAndRoutes, _results);
}

void DoseValidator::checkDoses(
        const Core::ParallelDosageSequence& _parallelDosageSequence,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    checkDosageBoundedList(_parallelDosageSequence.getDosageList(), _modelFormulationsAndRoutes, _results);
}

void DoseValidator::checkDosageBoundedList(
        const Core::DosageBoundedList& _dosageBoundedList,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    // For each dosage.
    for (const std::unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosageBoundedList) {
        checkDoses(*dosage, _modelFormulationsAndRoutes, _results);
    }
}

void DoseValidator::validateDose(
        const Core::DosageBounded* _dosage,
        const Core::FormulationAndRoute& _dosageFr,
        double _doseValue,
        const Common::TucuUnit& _doseUnit,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    auto compatibleIt =
            find_if(_modelFormulationsAndRoutes.getList().begin(),
                    _modelFormulationsAndRoutes.getList().end(),
                    [&_dosageFr](const std::unique_ptr<Core::FullFormulationAndRoute>& _ffr) {
                        return _ffr->getFormulationAndRoute().isCompatible(_dosageFr);
                    });

    if (compatibleIt == _modelFormulationsAndRoutes.getList().end()) {
        throw invalid_argument("No corresponding full formulation and route found for a dosage.");
    }

    const Core::FullFormulationAndRoute* modelFormAndRoute = compatibleIt->get();

    // Convert the dose value to match model formulation and route unit.
    double value =
            Common::UnitManager::convertToUnit(_doseValue, _doseUnit, modelFormAndRoute->getValidDoses()->getUnit());

    // Checking if limits are respected.
    std::string warning;
    auto const values = modelFormAndRoute->getValidDoses()->getValues();
    if (value < values.front()) {
        warning = LanguageManager::getInstance().translate("minimum_dosage_warning") + " ("
                  + doubleToString(values.front()) + " " + modelFormAndRoute->getValidDoses()->getUnit().toString()
                  + ")";
    }
    else if (value > values.back()) {
        warning = LanguageManager::getInstance().translate("maximum_dosage_warning") + " ("
                  + doubleToString(values.back()) + " " + modelFormAndRoute->getValidDoses()->getUnit().toString()
                  + ")";
    }

    _results.emplace(make_pair(_dosage, DoseValidationResult(_dosage, warning, WarningLevel::NORMAL)));
}

void DoseValidator::checkDoses(
        const Core::SingleDoseAtTimeList& _singleDoseAtTimeList,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    auto dosageList = _singleDoseAtTimeList.getDosageList();
    validateDose(
            &_singleDoseAtTimeList,
            dosageList.back().getFormulationAndRoute(),
            dosageList.back().getDoseValue(),
            dosageList.back().getDoseUnit(),
            _modelFormulationsAndRoutes,
            _results);
}

void DoseValidator::checkDoses(
        const Core::SimpleDoseList& _simpleDoseList,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        std::map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    auto dosageList = _simpleDoseList.getDosageList();
    validateDose(
            &_simpleDoseList,
            _simpleDoseList.getFormulationAndRoute(),
            dosageList.back().getDoseValue(),
            _simpleDoseList.getDoseUnit(),
            _modelFormulationsAndRoutes,
            _results);
}

void DoseValidator::checkDoses(
        const Core::SingleDose& _singleDose,
        const Core::FormulationAndRoutes& _modelFormulationsAndRoutes,
        map<const Core::DosageBounded*, DoseValidationResult>& _results) const
{
    validateDose(
            &_singleDose,
            _singleDose.getLastFormulationAndRoute(),
            _singleDose.getDose(),
            _singleDose.getDoseUnit(),
            _modelFormulationsAndRoutes,
            _results);
}

} // namespace Xpert
} // namespace Tucuxi
