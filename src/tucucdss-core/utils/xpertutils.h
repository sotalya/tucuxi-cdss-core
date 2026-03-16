// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef TUCUXI_XPERTUTILS_H
#define TUCUXI_XPERTUTILS_H

#include <string>

#include "tucucommon/datetime.h"
#include "tucucommon/translatablestring.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "tucucdss-core/language/languagemanager.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

#ifdef WIN32
//const char DIR_SEPARATOR = '\\';
const char DIR_SEPARATOR = '/';
#else
const char DIR_SEPARATOR = '/';
#endif // WIN32


/// \brief Convert a DataType to a string.
/// \param _dataType DataType to convert.
/// \return The string of the resulting conversion.
std::string dataTypeToString(const Core::DataType& _dataType);

/// \brief Convert a double to a string with two decimals.
/// \param _double Double to convert.
/// \return The string of the resulting conversion.
std::string doubleToString(const double& _double);

/// \brief Convert a CovariateType to a string.
/// \param _covariateType CovariateType to convert.
/// \return The string of the resulting conversion.
/// \throw invalid_argument If CovariateType is not supported.
std::string covariateTypeToString(CovariateType _covariateType);

/// \brief Convert an OutputLang to a string.
/// \param _lang OutputLang to convert.
/// \return The string of the resulting conversion.
/// \throw LanguageException if OutputLang is not supported.
std::string outputLangToString(OutputLang _lang);

/// \brief Convert a WarningLevel to a string.
/// \param _level WarningLevel to convert.
/// \return The string of the resulting conversion.
/// \throw invalid_argument If WarningLevel is not supported.
std::string warningLevelToString(WarningLevel _level);

/// \brief Convert a DateTime to a usable string for xml export.
/// \param _dateTime DateTime to convert.
/// \return The string of the resulting conversion.
std::string dateTimeToXmlString(const Common::DateTime& _dateTime);

/// \brief Convert a TimeOfDay to a string.
///        It removes the seconds.
///        It removes the minutes if they are equal to 0.
///        Suffix the hour acronym of the language manager to the hour value.
///        For example in English: 8h30 or 10h (when the minutes are equalt to 0).
/// \param _timeOfDay TimeOfDay to convert.
/// \return The string of the resulting conversion.
std::string timeOfDayToString(const Common::TimeOfDay& _timeOfDay);

/// \brief Convert a DateTime to a string.
///        Format: <day>.<month>.<year> <hour><hour acronym><minutes>
///        It removes the seconds.
///        It removes the minutes if they are equal to 0.
/// \param _dateTime DateTime to convert
/// \param _withTime Indicates whether the result string should include the time part.
///                  The default value is true.
/// \return The string of the resulting conversion.
std::string dateTimeToString(const DateTime& _dateTime, bool _withTime = true);

/// \brief Beautify a string that represents a covariate value.
///        If the DataType is Bool, returns the translation of "yes" or "no".
///        If the DataType is double, return the value with two decimals.
///        If the covariate identifier is sex, returns the translation of
///        "man", "woman" or "undefined".
/// \param _value Value of the covariate.
/// \param _type DataType of the covariate.
/// \param _id Identifier of the the covariate.
/// \return The beautified string or the unchanged input if it does not meet any beautify criteria.
std::string beautifyString(const std::string& _value, Core::DataType _type, const std::string& _id);

/// \brief Extract a translated string from a translatable string based on a given language.
///        If the language is not extractable, it tries to fall back on English.
/// \param _translatableString Translatable string.
/// \param _lang Lang to extract.
/// \return If present, the translated string, otherwise the English translation, otherwise an empty string.
std::string getStringWithEnglishFallback(const Common::TranslatableString& _translatableString, OutputLang _lang);

/// \brief Extract the start time of the dosage time range that is the oldest in the dosage history
///        and before the reference time.
/// \param _dosageHistory Dosage history to extract the start time of the oldest dosage time range.
/// \param _referenceTime Reference time. The resulting start time must be earlier than this time.
/// \return The oldest starting time found or the reference time if the dosage history is empty.
Common::DateTime getOldestDosageTimeRangeStart(
        const Core::DosageHistory& _dosageHistory, const Common::DateTime& _referenceTime);

/// \brief Extract the start of the most recent (last in the past/youngest) dosage time range that
///        is closest to the reference time.
/// \param _dosageHistory Dosage history to extract the start time of the latest dosage time range.
/// \param _referenceTime Reference time. The resulting start time must be earlier than this time.
/// \return The latest starting time found or an undefined date-time if the dosage history is empty or in the future.
Common::DateTime getLatestDosageTimeRangeStart(
        const Core::DosageHistory& _dosageHistory, const Common::DateTime& _referenceTime);

/// \brief Compute the final file name considering the desired output path and the format.
///        The final file name is <drugId>_<request number>_<computation time>.<file format>
/// \param _xpertRequestResult XpertRequestResult to get the output directory path, the drug id and the file format.
/// \param _addOutputPath Tell whether the path of the output directory should be prefixed to the file name.
/// \param _addExtension Tell whether the file extension should be suffixed to the file name.
/// \return Return the final file name.
std::string computeFileName(
        const XpertRequestResult& _xpertRequestResult, bool _addOutputPath = true, bool _addExtension = true);

/// \brief For the given trait T, make a request and execute it. Then convert the response to U and
///        place it in the response pointer.
///        The response pointer is set to nullptr if the computation fails.
/// \param _trait Computing trait to be used for request.
/// \param _xpertRequestResult XpertRequestResult to retrieve the treatment and drug model.
/// \param _responsePointer Pointer where to put the response.
template<typename T, typename U>
void executeRequestAndGetResult(
        std::unique_ptr<T> _trait, const XpertRequestResult& _xpertRequestResult, std::unique_ptr<U>& _responsePointer)
{
    // Make the computing request and response.
    Core::ComputingRequest computingRequest{
            "", *_xpertRequestResult.getDrugModel(), *_xpertRequestResult.getTreatment(), move(_trait)};
    std::unique_ptr<Core::ComputingResponse> computingResponse = std::make_unique<Core::ComputingResponse>("");

    // Start the computation in tucuxi-core.
    Core::IComputingService* computingComponent =
            dynamic_cast<Core::IComputingService*>(Core::ComputingComponent::createComponent());
    Core::ComputingStatus result = computingComponent->compute(computingRequest, computingResponse);

    delete computingComponent;

    // If the computation failed, set to nullptr and leave.
    if (result != Core::ComputingStatus::Ok) {
        _responsePointer = nullptr;
        return;
    }

    // Acquire the data and put it in the unique pointer as the response.
    U* dataPointer = dynamic_cast<U*>(computingResponse->getUniquePointerData().release());
    _responsePointer = std::unique_ptr<U>(dataPointer);
}


template<typename T, typename U>
void executeRequestAndGetResult(
        std::unique_ptr<T> _trait,
        const XpertRequestResult& _xpertRequestResult,
        const Core::DrugTreatment& _treatment,
        std::unique_ptr<U>& _responsePointer)
{
    // Make the computing request and response.
    Core::ComputingRequest computingRequest{"", *_xpertRequestResult.getDrugModel(), _treatment, move(_trait)};
    std::unique_ptr<Core::ComputingResponse> computingResponse = std::make_unique<Core::ComputingResponse>("");

    // Start the computation in tucuxi-core.
    Core::IComputingService* computingComponent =
            dynamic_cast<Core::IComputingService*>(Core::ComputingComponent::createComponent());
    Core::ComputingStatus result = computingComponent->compute(computingRequest, computingResponse);

    delete computingComponent;

    // If the computation failed, set to nullptr and leave.
    if (result != Core::ComputingStatus::Ok) {
        _responsePointer = nullptr;
        return;
    }

    // Acquire the data and put it in the unique pointer as the response.
    U* dataPointer = dynamic_cast<U*>(computingResponse->getUniquePointerData().release());
    _responsePointer = std::unique_ptr<U>(dataPointer);
}

/// \brief Convert a camel case key to a phrase.
///        "camelCaseKey" becomes "Camel case key".
/// \param key Camel case ke to convert.
/// \return The resulting phrase.
std::string keyToPhrase(const std::string& _key);

/// \brief For a given date of birth and computation time, compute the age between these two dates
///        according to a given age type.
/// \param _ageType Age type to compute.
/// \param _birthDate Date of birth.
/// \param _computationTime Computation time to use as the present day.
/// \return The resulting age as a double.
/// \throw invalid_argument When the covariate type is not an age type.
double getAgeIn(
        Core::CovariateType _ageType, const Common::DateTime& _birthDate, const Common::DateTime& _computationTime);


} // namespace Xpert
} // namespace Tucuxi

#endif // TUCUXI_XPERTUTILS_H
