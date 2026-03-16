// SPDX-License-Identifier: AGPL-3.0-or-later
#include <memory>

#include "justificationcreator.h"

#include "query/justification.h"
#include "tucucdss-core/language/languagemanager.h"
#include "utils/xpertutils.h"

namespace Tucuxi {
namespace Xpert {

void JustificationCreator::perform(XpertRequestResult& _xpertRequestResult)
{
    if (_xpertRequestResult.getAdjustmentData() == nullptr) {
        return;
    }

    Justification justification;
    DosageInfo oldDosage;
    DosageInfo newDosage;
    DosageInfo firstDosage;

    Core::DosageAdjustment bestAdj = getBestAdjustment(*_xpertRequestResult.getAdjustmentData());

    getAbstractDosage(*bestAdj.getDosageHistory().getDosageTimeRanges().at(0)->getDosage(), firstDosage);

    if (_xpertRequestResult.getTreatment()->getDosageHistory().isEmpty()) {
        oldDosage.dose = -1;
    }
    else {
        getAbstractDosage(
                *_xpertRequestResult.getTreatment()->getDosageHistory().getDosageTimeRanges().at(0)->getDosage(),
                oldDosage);
    }

    setJustificationExposure(justification, _xpertRequestResult, bestAdj);

    justification.setFirstDoseText(firstDosage.doseText);
    justification.setFirstDoseDate(bestAdj.getDosageHistory().getDosageTimeRanges().at(0)->getStartDate());
    justification.setFirstDoseValue(firstDosage.dose);

    if (bestAdj.getDosageHistory().getDosageTimeRanges().size() == 2) {
        getAbstractDosage(*bestAdj.getDosageHistory().getDosageTimeRanges().at(1)->getDosage(), newDosage);
        justification.setJustificationType(JustificationType::DOUBLE);

        setJustificationDose(justification, oldDosage.dose, newDosage.dose);

        treatJustificationIntervalInfo(justification, oldDosage, newDosage);
        justification.setSecondDoseText(newDosage.doseText);
        justification.setSecondDoseDate(bestAdj.getDosageHistory().getDosageTimeRanges().at(1)->getStartDate());
    }
    else {
        justification.setJustificationType(JustificationType::SIMPLE);
        setJustificationDose(justification, oldDosage.dose, firstDosage.dose);
        treatJustificationIntervalInfo(justification, oldDosage, firstDosage);
    }


    _xpertRequestResult.setJustification(justification);
}

Core::DosageAdjustment JustificationCreator::getBestAdjustment(const Core::AdjustmentData& _adjustmentData)
{
    Core::DosageAdjustment bestAdj;
    double highestScore = -1;

    for (const auto& adj : _adjustmentData.getAdjustments()) {
        if (adj.getGlobalScore() > highestScore) {
            bestAdj = adj;
            highestScore = adj.getGlobalScore();
        }
    }
    return bestAdj;
}

void JustificationCreator::setJustificationExposure(
        Justification& _justification,
        const XpertRequestResult& _xpertRequestResult,
        const Core::DosageAdjustment& _bestAdj)
{
    if (_xpertRequestResult.getAdjustmentData()->getCurrentDosageWithScore().m_targetsEvaluation.empty()) {
        return;
    }

    double targetBefore =
            _xpertRequestResult.getAdjustmentData()->getCurrentDosageWithScore().m_targetsEvaluation.at(0).getValue();
    double targetBest = _bestAdj.m_targetsEvaluation.at(0).getTarget().getValueBest();

    if (targetBefore < targetBest) {
        _justification.setJustificationExposureSign(JustificationExposureSign::BELOW);
    }
    else if (targetBefore > targetBest) {
        _justification.setJustificationExposureSign(JustificationExposureSign::ABOVE);
    }
    else {
        _justification.setJustificationExposureSign(JustificationExposureSign::EQUAL);
    }
}

void JustificationCreator::setJustificationDose(Justification& _justification, double _oldDose, double _newDose)
{
    if (_oldDose == -1) {
        _justification.setJustificationDoseSign(JustificationDoseSign::NEW);
        return;
    }

    if (_oldDose > _newDose) {
        _justification.setJustificationDoseSign(JustificationDoseSign::DECREASE);
    }
    else if (_oldDose < _newDose) {
        _justification.setJustificationDoseSign(JustificationDoseSign::INCREASE);
    }
    else {
        _justification.setJustificationDoseSign(JustificationDoseSign::EQUAL);
    }
}

int interpretLastingInterval(double _timeInterval)
{
    if (_timeInterval == 24.0) {
        return 1;
    }
    if (_timeInterval > 24.0 && _timeInterval < 168.0) {
        return 2;
    }
    if (_timeInterval >= 168.0) {
        return 3;
    }
    return 0;
}

int getComparableInterval(const DosageInfo& _dosage)
{
    switch (_dosage.interval) {
    case Interval::DAILY:
        return 1;
    case Interval::WEEKLY:
        return 3;
    case Interval::LASTING:
        return interpretLastingInterval(_dosage.timeInterval);
    }
    return -1;
}

JustificationInterval compareIntervalTypes(const DosageInfo& _oldDosage, const DosageInfo& _newDosage)
{
    if (_oldDosage.interval == Interval::LASTING && _newDosage.interval == Interval::LASTING) {
        if (_oldDosage.timeInterval < _newDosage.timeInterval) {
            return JustificationInterval::HIGHER;
        }
        if (_oldDosage.timeInterval > _newDosage.timeInterval) {
            return JustificationInterval::LOWER;
        }
        return JustificationInterval::EQUAL;
    }

    auto oldInterval = getComparableInterval(_oldDosage);
    auto newInterval = getComparableInterval(_newDosage);

    if (oldInterval < newInterval) {
        return JustificationInterval::HIGHER;
    }
    if (oldInterval > newInterval) {
        return JustificationInterval::LOWER;
    }

    return JustificationInterval::EQUAL;
}

void JustificationCreator::treatJustificationIntervalInfo(
        Justification& _justification, const DosageInfo& _oldDosage, const DosageInfo& _newDosage)
{
    JustificationInterval intervalSign;
    if (_oldDosage.dose == -1) {
        intervalSign = JustificationInterval::NEW;
    }
    else {
        intervalSign = compareIntervalTypes(_oldDosage, _newDosage);
    }
    _justification.setJustificationInterval(intervalSign);
}

void JustificationCreator::getSingleDose(const Core::SingleDose& _dosage, DosageInfo& _dosageInfo) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();
    static std::map<Tucuxi::Core::AdministrationRoute, std::string> routes = {
            {Tucuxi::Core::AdministrationRoute::Oral, "oral"},
            {Tucuxi::Core::AdministrationRoute::Nasal, "nasal"},
            {Tucuxi::Core::AdministrationRoute::Rectal, "rectal"},
            {Tucuxi::Core::AdministrationRoute::Vaginal, "vaginal"},
            {Tucuxi::Core::AdministrationRoute::Undefined, "undefined"},
            {Tucuxi::Core::AdministrationRoute::Sublingual, "sublingual"},
            {Tucuxi::Core::AdministrationRoute::Transdermal, "transdermal"},
            {Tucuxi::Core::AdministrationRoute::Subcutaneous, "subcutaneous"},
            {Tucuxi::Core::AdministrationRoute::Intramuscular, "intramuscular"},
            {Tucuxi::Core::AdministrationRoute::IntravenousDrip, "intravenous_drip"},
            {Tucuxi::Core::AdministrationRoute::IntravenousBolus, "intravenous_bolus"}};

    auto it = routes.find(_dosage.getLastFormulationAndRoute().getAdministrationRoute());

    std::stringstream dosageStream;
    dosageStream << doubleToString(_dosage.getDose()) << " " << _dosage.getDoseUnit().toString();

    if (it != routes.end() && it->second != "undefined") {
        dosageStream << " (" << langMgr.translate(it->second) << ")";
    }

    if (!_dosageInfo.doseText.empty()) {
        _dosageInfo.doseText = dosageStream.str() + ", " + _dosageInfo.doseText;
    }
    else {
        _dosageInfo.doseText = dosageStream.str();
    }

    _dosageInfo.dose = _dosage.getDose();
}

void JustificationCreator::getDosage(const Core::DosageLoop& _dosage, DosageInfo& _dosageInfo) const
{
    getAbstractDosage(*_dosage.getDosage(), _dosageInfo);
}

void JustificationCreator::getDosage(const Core::DosageSteadyState& _dosage, DosageInfo& _dosageInfo) const
{
    getAbstractDosage(*_dosage.getDosage(), _dosageInfo);
}

void JustificationCreator::getDosage(const Core::DosageRepeat& _dosage, DosageInfo& _dosageInfo) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();
    std::stringstream dosageStream;
    dosageStream << _dosage.getNbTimes() << " " << langMgr.translate("times");

    if (!_dosageInfo.doseText.empty()) {
        _dosageInfo.doseText = dosageStream.str() + ", " + _dosageInfo.doseText;
    }
    else {
        _dosageInfo.doseText = dosageStream.str();
    }

    getAbstractDosage(*_dosage.getDosage(), _dosageInfo);
}

void JustificationCreator::getDosage(const Core::DosageSequence& _dosage, DosageInfo& _dosageInfo) const
{
    for (const std::unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosage.getDosageList()) {
        getAbstractDosage(*dosage, _dosageInfo);
    }
}

void JustificationCreator::getDosage(const Core::ParallelDosageSequence& _dosage, DosageInfo& _dosageInfo) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();
    auto timeOffsetIt = _dosage.getOffsetsList().begin();

    for (const std::unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosage.getDosageList()) {
        std::stringstream offsetStream;
        offsetStream << langMgr.translate("offset") << " "
                     << timeOfDayToString(TimeOfDay::buildUnnormalized(*timeOffsetIt));

        if (!_dosageInfo.doseText.empty()) {
            _dosageInfo.doseText = offsetStream.str() + ", " + _dosageInfo.doseText;
        }
        else {
            _dosageInfo.doseText = offsetStream.str();
        }

        getAbstractDosage(*dosage, _dosageInfo);
        ++timeOffsetIt;
    }
}

void JustificationCreator::getDosage(const Core::LastingDose& _dosage, DosageInfo& _dosageInfo) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    std::stringstream dosageStream;
    dosageStream << langMgr.translate("interval") << " "
                 << timeOfDayToString(TimeOfDay::buildUnnormalized(_dosage.getTimeStep()));

    if (!_dosageInfo.doseText.empty()) {
        _dosageInfo.doseText = dosageStream.str() + ", " + _dosageInfo.doseText;
    }
    else {
        _dosageInfo.doseText = dosageStream.str();
    }
    _dosageInfo.interval = Interval::LASTING;
    _dosageInfo.timeInterval = _dosage.getTimeStep().toHours();

    getSingleDose(_dosage, _dosageInfo);
}

void JustificationCreator::getDosage(const Core::DailyDose& _dosage, DosageInfo& _dosageInfo) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    std::stringstream dosageStream;
    dosageStream << langMgr.translate("daily_at") << " " << timeOfDayToString(_dosage.getTimeOfDay());

    if (!_dosageInfo.doseText.empty()) {
        _dosageInfo.doseText = dosageStream.str() + ", " + _dosageInfo.doseText;
    }
    else {
        _dosageInfo.doseText = dosageStream.str();
    }

    _dosageInfo.interval = Interval::DAILY;
    getSingleDose(_dosage, _dosageInfo);
}

void JustificationCreator::getDosage(const Core::WeeklyDose& _dosage, DosageInfo& _dosageInfo) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    std::stringstream dosageStream;
    dosageStream << langMgr.translate("every") << " "
                 << langMgr.translate("day_" + std::to_string(_dosage.getDayOfWeek().iso_encoding())) << " "
                 << langMgr.translate("at") << " " << timeOfDayToString(_dosage.getTimeOfDay());

    if (!_dosageInfo.doseText.empty()) {
        _dosageInfo.doseText = dosageStream.str() + ", " + _dosageInfo.doseText;
    }
    else {
        _dosageInfo.doseText = dosageStream.str();
    }
    _dosageInfo.interval = Interval::WEEKLY;
    getSingleDose(_dosage, _dosageInfo);
}

#define TRY_GET_JSON(Type)                                                                 \
    if (dynamic_cast<const Tucuxi::Core::Type*>(&_dosage)) {                               \
        return getDosage(*dynamic_cast<const Tucuxi::Core::Type*>(&_dosage), _dosageInfo); \
    }


void JustificationCreator::getAbstractDosage(const Core::Dosage& _dosage, DosageInfo& _dosageInfo) const
{
    TRY_GET_JSON(WeeklyDose);
    TRY_GET_JSON(DailyDose);
    TRY_GET_JSON(LastingDose);
    TRY_GET_JSON(ParallelDosageSequence);
    TRY_GET_JSON(DosageLoop);
    TRY_GET_JSON(DosageSteadyState);
    TRY_GET_JSON(DosageRepeat);
    TRY_GET_JSON(DosageSequence);
}

std::string JustificationCreator::prefixDosage(
        const std::string& _dosageIndication, const std::string& _dosageIndicationChain) const
{
    std::stringstream newDosageIndicationsChainStream;
    newDosageIndicationsChainStream << _dosageIndication // Add a coma only if there is already a dosage indication.
                                    << (_dosageIndicationChain.empty() ? "" : ", ") << _dosageIndicationChain;

    return newDosageIndicationsChainStream.str();
}


} // namespace Xpert
} // namespace Tucuxi
