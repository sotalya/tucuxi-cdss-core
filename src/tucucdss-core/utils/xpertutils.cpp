// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cctype>
#include <iomanip>
#include <sstream>

#include "xpertutils.h"

#include "tucucdss-core/language/languageexception.h"
#include "tucucdss-core/result/xpertqueryresult.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

std::string dataTypeToString(const Core::DataType& _dataType)
{
    switch (_dataType) {
    case Core::DataType::Int: {
        return "int";
    } break;

    case Core::DataType::Double: {
        return "double";
    } break;

    case Core::DataType::Bool: {
        return "bool";
    } break;

    case Core::DataType::Date: {
        return "date";
    } break;

    default:
        throw invalid_argument("unknown data type"); // If properly  maintained, should never be used.
    }
}

std::string doubleToString(const double& _double)
{
    stringstream stream;
    stream << fixed << setprecision(2) << _double;
    std::string i = stream.str();
    return stream.str();
}

std::string covariateTypeToString(CovariateType _covariateType)
{
    switch (_covariateType) {
    case CovariateType::MODEL:
        return "default";
    case CovariateType::PATIENT:
        return "patient";
    default:
        throw invalid_argument("Unknown covariate type"); // If properly  maintained, should never be used.
    }
}

std::string outputLangToString(OutputLang _lang)
{
    switch (_lang) {
    case OutputLang::ENGLISH:
        return "en";
    case OutputLang::FRENCH:
        return "fr";
    default:
        throw LanguageException("Unknown language"); // If properly  maintained, should never be used.
    }
}

std::string warningLevelToString(WarningLevel _level)
{
    switch (_level) {
    case WarningLevel::NORMAL:
        return "normal";
    case WarningLevel::WARNING:
        return "warning";
    case WarningLevel::CRITICAL:
        return "critical";
    default:
        throw invalid_argument("Unknown warning level"); // If properly  maintained, should never be used.
    }
}

std::string dateTimeToXmlString(const Tucuxi::Common::DateTime& _dateTime)
{
    if (_dateTime.isUndefined()) {
        return "";
    }

    std::string result;
    result = std::to_string(_dateTime.year()) + "." + std::to_string(_dateTime.month()) + "."
             + std::to_string(_dateTime.day()) + "T" + std::to_string(_dateTime.hour()) + ":"
             + std::to_string(_dateTime.minute()) + ":" + std::to_string(_dateTime.second());

    char str[20]; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    snprintf(
            str,
            20,
            "%04d-%02d-%02dT%02d:%02d:%02d",
            _dateTime.year(),
            _dateTime.month(),
            _dateTime.day(),
            _dateTime.hour(),
            _dateTime.minute(),
            _dateTime.second());
    result = str;
    return result;
}

std::string timeOfDayToString(const Common::TimeOfDay& _timeOfDay)
{

    stringstream timeStream;

    LanguageManager& langMgr = LanguageManager::getInstance(); //  XXXXX Does not work here

    timeStream << _timeOfDay.hour() << langMgr.translate("hour_acronym");

    if (_timeOfDay.minute() != 0) {
        timeStream << _timeOfDay.minute();
    }

    return timeStream.str();
}

std::string dateTimeToString(const Common::DateTime& _dateTime, bool _withTime)
{
    if (_dateTime.isUndefined()) {
        return "";
    }

    stringstream dateTimeStream;


    // Setting the date part.
    dateTimeStream << _dateTime.day() << '.' << _dateTime.month() << '.' << _dateTime.year();

    if (!_withTime) {
        return dateTimeStream.str();
    }

    // Setting the time part.
    dateTimeStream << ' '
                   << timeOfDayToString(Common::Duration(
                              chrono::hours(_dateTime.hour()),
                              chrono::minutes(_dateTime.minute()),
                              chrono::seconds(_dateTime.second())));

    return dateTimeStream.str();
}

std::string beautifyString(const std::string& _value, Core::DataType _type, const std::string& _id)
{
    Tucuxi::Common::LoggerHelper logHelper;
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Handle empty values --- not doing so would result in the code crashing
    // badly.
    if (_value.empty()) {
        return "";
    }

    try {
        // Convert the value to yes/no for nice display if the data type is bool.
        if (_type == Core::DataType::Bool) {
            if (stoi(_value) != 0) {
                return langMgr.translate("yes");
            }
            return langMgr.translate("no");
        }
        // Only print two decimals if it is a double.
        if (_type == Core::DataType::Double && _id != "sex") {
            return doubleToString(stod(_value));
        }
        if (_type == Core::DataType::Int) {
            return std::to_string(stoi(_value));
        }

        // Convert the value to man/woman/undefined for nice display.
        if (_id == "sex") {
            double sexAsDouble = stod(_value);
            if (sexAsDouble >= 0 && sexAsDouble < 0.4) {
                return langMgr.translate("woman");
            }
            if (sexAsDouble <= 1 && sexAsDouble > 0.6) {
                return langMgr.translate("man");
            }
            return langMgr.translate("undefined");
        }
    }
    catch (std::invalid_argument const& e) {
        logHelper.error("Invalid conversion to numeric type from value " + _value);
        return "";
    }

    return _value;
}

// Lang should be ENGLISH
std::string getStringWithEnglishFallback(const Common::TranslatableString& _translatableString, OutputLang _lang)
{
    std::string translatedString = _translatableString.getString(outputLangToString(_lang));

    // Required translation.
    if (!translatedString.empty()) {
        return translatedString;
    }
    // English or empty string.
    return _translatableString.getString();
}

Common::DateTime getOldestDosageTimeRangeStart(
        const Core::DosageHistory& _dosageHistory, const Common::DateTime& _referenceTime)
{
    // In case the dosage history is empty. The olest dosage time range
    // start date is the reference time.
    Common::DateTime oldestDateKnown = _referenceTime;

    // Iterate over the time ranges and find the one that is the oldest.
    for (const unique_ptr<Core::DosageTimeRange>& timeRange : _dosageHistory.getDosageTimeRanges()) {
        if (timeRange->getStartDate() < oldestDateKnown) {
            oldestDateKnown = timeRange->getStartDate();
        }
    }

    return oldestDateKnown;
}

Common::DateTime getLatestDosageTimeRangeStart(
        const Core::DosageHistory& _dosageHistory, const Common::DateTime& _referenceTime)
{
    // In case the dosage history is empty. The latest dosage time range start date is undefined.
    Common::DateTime latestDateKnown = Common::DateTime::undefinedDateTime();

    // Iterate over the time ranges and find the one that is the latest.
    for (const unique_ptr<Core::DosageTimeRange>& timeRange : _dosageHistory.getDosageTimeRanges()) {
        if (timeRange->getStartDate() < _referenceTime
            && (latestDateKnown.isUndefined() || timeRange->getStartDate() > latestDateKnown)) {
            latestDateKnown = timeRange->getStartDate();
        }
    }

    return latestDateKnown;
}

std::string computeFileName(const XpertRequestResult& _xpertRequestResult, bool _addOutputPath, bool _addExtension)
{
    stringstream fileNameStream;
    Common::DateTime computationTime = _xpertRequestResult.getXpertQueryResult().getComputationTime();

    // If the output path should be prefixed.
    if (_addOutputPath) {
        fileNameStream << _xpertRequestResult.getXpertQueryResult().getOutputPath() << DIR_SEPARATOR;
    }

    fileNameStream << _xpertRequestResult.getXpertRequest().getDrugId() << "_"
                   << _xpertRequestResult.getXpertQueryResult().getRequestIndexBeingProcessed() + 1 << "_"
                   << computationTime.day() << "-" << computationTime.month() << "-" << computationTime.year() << "_"
                   << computationTime.hour() << "h" << computationTime.minute() << "m" << computationTime.second()
                   << "s";

    // If the extension should be suffixed.
    if (_addExtension) {

        std::string extension;

        switch (_xpertRequestResult.getXpertRequest().getOutputFormat()) {
        case OutputFormat::XML:
            extension = "xml";
            break;
        case OutputFormat::HTML:
            extension = "html";
            break;
        case OutputFormat::PDF:
            extension = "pdf";
            break;
        }

        fileNameStream << "." << extension;
    }

    return fileNameStream.str();
}

std::string keyToPhrase(const std::string& _key)
{

    stringstream phraseStream;

    // Traverse the key.
    for (size_t i = 0; i < _key.length(); i++) {
        // If the first char is lowercase, change it to upper case.
        if (i == 0 && (islower(_key[i]) != 0)) {
            phraseStream << static_cast<char>(toupper(_key[i]));
        }

        // Convert to lower case if it's
        // an upper case character but not the first.
        else if ((isupper(_key[i]) != 0) && i != 0) {
            // phraseStream << ' ' << char(tolower(_key[i]));  XXXXX Why would we want this?
            phraseStream << static_cast<char>(tolower(_key[i]));
        }

        // Finally, if it's a lower case character,
        // then just add it to the phrase.
        else {
            phraseStream << _key[i];
        }
    }

    return phraseStream.str();
}

double getAgeIn(
        Core::CovariateType _ageType, const Common::DateTime& _birthDate, const Common::DateTime& _computationTime)
{
    switch (_ageType) {
    case Core::CovariateType::AgeInDays:
        return static_cast<double>(Common::Utils::dateDiffInDays(_birthDate, _computationTime));
        break;
    case Core::CovariateType::AgeInWeeks:
        return static_cast<double>(Common::Utils::dateDiffInWeeks(_birthDate, _computationTime));
        break;
    case Core::CovariateType::AgeInMonths:
        return static_cast<double>(Common::Utils::dateDiffInMonths(_birthDate, _computationTime));
        break;
    case Core::CovariateType::AgeInYears:
        return static_cast<double>(Common::Utils::dateDiffInYears(_birthDate, _computationTime));
        break;
    default:
        throw invalid_argument("Invalid covariate type.");
    }
}

} // namespace Xpert
} // namespace Tucuxi
