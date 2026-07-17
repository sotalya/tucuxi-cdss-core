// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <memory>

#include <spdlog/fmt/fmt.h>

#include "justificationcreator.h"

#include "query/justification.h"
#include "tucucdss-core/language/languagemanager.h"
#include "utils/xpertutils.h"

namespace Tucuxi {
namespace Xpert {

namespace {

/// \brief Number of hours in one day, used to normalise a regimen to a 24 h exposure.
constexpr double HOURS_PER_DAY = 24.0;

/// \brief Number of hours in one week, used to normalise a weekly regimen to a 24 h exposure.
constexpr double HOURS_PER_WEEK = 168.0;

/// \brief Absolute tolerance under which two normalised exposures are considered equal, so that floating point noise
///        does not flip the direction.
constexpr double DAILY_DOSE_EPSILON = 1e-6;

/// \brief Compute a regimen's drug exposure normalised to a 24 h period, in the dose unit of the regimen:
///        exposure = unit_dose * (24 / tau_hours).
///        This value is used ONLY to decide the direction of the recommendation (increase, decrease, unchanged); it is
///        never displayed, because for a regimen that is not daily (for example weekly) it is a per-day figure that is
///        not taken on any single day. The report shows the actual regimens instead.
/// \param _dosage Abstract dosage information (unit dose and interval).
/// \return The 24 h normalised exposure. Falls back to the unit dose when the interval cannot yield a meaningful
///         period.
double computeDailyDose(const DosageInfo& _dosage)
{
    switch (_dosage.interval) {
    case Interval::DAILY:
        // One administration every 24 h: the daily dose is the unit dose.
        return _dosage.dose;
    case Interval::WEEKLY:
        // One administration every 7 days.
        return _dosage.dose * HOURS_PER_DAY / HOURS_PER_WEEK;
    case Interval::LASTING:
        // One administration every tau hours.
        if (_dosage.timeInterval > 0.0) {
            return _dosage.dose * HOURS_PER_DAY / _dosage.timeInterval;
        }
        return _dosage.dose;
    }
    return _dosage.dose;
}

/// \brief Format a numeric value without trailing zeros, so that a whole value renders without a decimal part (450.00
///        becomes "450") while a fractional value keeps only the digits it needs (12.50 becomes "12.5").
/// \param _value Value to format.
/// \return The trimmed textual representation.
std::string formatTrimmed(double _value)
{
    std::string text = doubleToString(_value);
    std::string::size_type dotPos = text.find('.');
    if (dotPos == std::string::npos) {
        return text;
    }

    std::string::size_type lastNonZero = text.find_last_not_of('0');
    // Drop the decimal point as well when no fractional digit remains.
    if (text[lastNonZero] == '.') {
        --lastNonZero;
    }
    text.erase(lastNonZero + 1);
    return text;
}

/// \brief Render the interval of a lasting dose as a natural phrase. A whole number of days is expressed in days
///        ("every day", "every 2 days", and so a monthly regimen reads "every 30 days"); anything else falls back to
///        hours ("every 12 h").
/// \param _hours Interval of the lasting dose, in hours.
/// \param _langMgr Language manager used to translate the phrase.
/// \return The interval phrase, or an empty string when the period is unknown.
std::string formatLastingInterval(double _hours, LanguageManager& _langMgr)
{
    if (_hours <= 0.0) {
        return "";
    }

    // A whole number of days reads more naturally as days than as hours.
    if (std::fmod(_hours, HOURS_PER_DAY) == 0.0) {
        double days = _hours / HOURS_PER_DAY;
        if (days == 1.0) {
            return _langMgr.translate("every_day");
        }
        return fmt::format(_langMgr.translate("every_days"), fmt::arg("count", formatTrimmed(days)));
    }

    return fmt::format(_langMgr.translate("every_hours"), fmt::arg("count", formatTrimmed(_hours)));
}

/// \brief Render a regimen as its actual intake, "<dose> <unit> <interval>", for instance "400 mg every 12 h", "450 mg
///         every day" or "800 mg every week".
/// \param _dosage Abstract dosage information (dose, unit and interval).
/// \return The human-readable regimen text.
std::string formatRegimen(const DosageInfo& _dosage)
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    std::string intervalPhrase;
    switch (_dosage.interval) {
    case Interval::DAILY:
        intervalPhrase = langMgr.translate("every_day");
        break;
    case Interval::WEEKLY:
        intervalPhrase = langMgr.translate("every_week");
        break;
    case Interval::LASTING:
        intervalPhrase = formatLastingInterval(_dosage.timeInterval, langMgr);
        break;
    }

    std::stringstream regimen;
    regimen << formatTrimmed(_dosage.dose) << " " << _dosage.doseUnit;

    // Omit the interval only when it is not known (a lasting dose with no period), in which case just the dose and
    // unit are shown.
    if (!intervalPhrase.empty()) {
        regimen << " " << intervalPhrase;
    }
    return regimen.str();
}

} // namespace

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

        setJustificationDose(justification, oldDosage, newDosage);

        treatJustificationIntervalInfo(justification, oldDosage, newDosage);
        justification.setSecondDoseText(newDosage.doseText);
        justification.setSecondDoseDate(bestAdj.getDosageHistory().getDosageTimeRanges().at(1)->getStartDate());
    }
    else {
        justification.setJustificationType(JustificationType::SIMPLE);
        setJustificationDose(justification, oldDosage, firstDosage);
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

void JustificationCreator::setJustificationDose(
        Justification& _justification, const DosageInfo& _oldDosage, const DosageInfo& _newDosage)
{
    // A new treatment has no previous regimen to compare against.
    if (_oldDosage.dose == -1) {
        _justification.setJustificationDoseSign(JustificationDoseSign::NEW);
        return;
    }

    // The direction of the recommendation is decided on the overall drug exposure, obtained by normalising each
    // regimen to a 24 h period, not on the unit dose. A unit dose can fall while the overall exposure rises, for
    // instance 450 mg every 24 h becoming 400 mg every 12 h, so only the normalised comparison gives the true
    // direction. The regimens themselves, the actual dose taken at once and its interval, are reported as they stand
    // stand, so no averaged figure is shown.
    double exposureBefore = computeDailyDose(_oldDosage);
    double exposureAfter = computeDailyDose(_newDosage);

    _justification.setPreviousRegimen(formatRegimen(_oldDosage));
    _justification.setRecommendedRegimen(formatRegimen(_newDosage));

    if (exposureBefore - exposureAfter > DAILY_DOSE_EPSILON) {
        _justification.setJustificationDoseSign(JustificationDoseSign::DECREASE);
    }
    else if (exposureAfter - exposureBefore > DAILY_DOSE_EPSILON) {
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
    _dosageInfo.doseUnit = _dosage.getDoseUnit().toString();
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


template<typename T>
bool JustificationCreator::tryGetDosage(const Core::Dosage& _dosage, DosageInfo& _dosageInfo) const
{
    if (const auto* typedDosage = dynamic_cast<const T*>(&_dosage)) {
        getDosage(*typedDosage, _dosageInfo);
        return true;
    }
    return false;
}

void JustificationCreator::getAbstractDosage(const Core::Dosage& _dosage, DosageInfo& _dosageInfo) const
{
    if (tryGetDosage<Core::WeeklyDose>(_dosage, _dosageInfo)) {
        return;
    }
    if (tryGetDosage<Core::DailyDose>(_dosage, _dosageInfo)) {
        return;
    }
    if (tryGetDosage<Core::LastingDose>(_dosage, _dosageInfo)) {
        return;
    }
    if (tryGetDosage<Core::ParallelDosageSequence>(_dosage, _dosageInfo)) {
        return;
    }
    if (tryGetDosage<Core::DosageLoop>(_dosage, _dosageInfo)) {
        return;
    }
    if (tryGetDosage<Core::DosageSteadyState>(_dosage, _dosageInfo)) {
        return;
    }
    if (tryGetDosage<Core::DosageRepeat>(_dosage, _dosageInfo)) {
        return;
    }
    if (tryGetDosage<Core::DosageSequence>(_dosage, _dosageInfo)) {
        return;
    }
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
