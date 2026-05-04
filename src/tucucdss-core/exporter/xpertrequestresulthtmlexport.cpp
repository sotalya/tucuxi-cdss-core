// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>
#include <fstream>
#include <sstream>

#include "xpertrequestresulthtmlexport.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/dosage.h"

#include "tucucdss-core/exporter/static/filestring.h"
#include "tucucdss-core/language/languagemanager.h"
#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

void XpertRequestResultHtmlExport::exportToFile(XpertRequestResult& _xpertRequestResult)
{

    // Get the filename <drugId>_<requestNumber>_<current time to evit conflict naming>.<file extension>
    std::string fileName = computeFileName(_xpertRequestResult);
    exportToFile(fileName, _xpertRequestResult);
}

void XpertRequestResultHtmlExport::exportToFile(const std::string& _fileName, XpertRequestResult& _xpertRequestResult)
{
    // Open the file.
    bool isJsTemplate = false;
    ofstream fileStream;
    fileStream.open(_fileName);
    if ((fileStream.rdstate() & ostream::failbit) != 0) {
        _xpertRequestResult.setErrorMessage("The file " + _fileName + " could not be opened.");
        return;
    }
    Tucuxi::Common::LoggerHelper logHelper;

    std::string ending = "_js.html";
    std::string ending2 = ".html";
    std::filesystem::path templateDir =
            std::filesystem::path(_xpertRequestResult.getXpertQueryResult().getTemplatePath())
            / std::filesystem::path(_xpertRequestResult.getXpertQueryResult().getTemplateName());
    std::string templateFilePath;

    for (const auto& entry : std::filesystem::directory_iterator(templateDir)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();

            if (std::equal(ending2.rbegin(), ending2.rend(), filePath.rbegin())) {
                templateFilePath = filePath;
                if (std::equal(ending.rbegin(), ending.rend(), filePath.rbegin())) {
                    isJsTemplate = true;
                }
                break;
            }
        }
    }

    logHelper.info("Path : {}", templateFilePath);

    m_xpertRequestResultInUse = &_xpertRequestResult;

    // Maybe we didn't manage to set up a correct html template. (Just in case)
    try {

        fileStream << makeHtmlString(_xpertRequestResult, templateFilePath, isJsTemplate);
    }
    catch (inja::RenderError& e) {
        _xpertRequestResult.setErrorMessage("Failed to render html: " + std::string(e.what()));
        return;
    }

    fileStream.close();
}

std::string XpertRequestResultHtmlExport::makeHtmlString(
        const XpertRequestResult& _xpertRequestResult, const std::string& _templatePath, bool _isJsTemplate) const
{
    Tucuxi::Common::LoggerHelper logHelper;

    std::ifstream templateFile(_templatePath);
    std::stringstream templateStream;

    if (templateFile) {
        templateStream << templateFile.rdbuf();
        templateFile.close();
    }
    else {
        exit(1);
    }

    inja::json json;

    json["css"]["normalize"] = FileString::NORMALIZE_CSS_STR;
    json["css"]["tuberxpert"] = FileString::TUBERXPERT_CSS_STR;

    json["js"]["graphing"] = FileString::GRAPHING_JS_STR;
    json["js"]["compgraph"] = FileString::COMPGRAPH_JS_STR;
    json["js"]["graphdata"] = FileString::GRAPHDATA_JS_STR;
    json["js"]["tuberxpert"] = FileString::TUBERXPERT_JS_STR;

    if (!_xpertRequestResult.shouldContinueProcessing()) {
        json["error_message"] = _xpertRequestResult.getErrorMessage();
    }

    getHeaderJson(_xpertRequestResult, json["header"]);
    getTranslationReportJson(_xpertRequestResult, json["report_translation"]);
    getDrugIntroJson(_xpertRequestResult, json["intro"]);
    getContactsJson(_xpertRequestResult.getXpertQueryResult().getAdminData(), json["contacts"]);
    getClinicalDatasJson(_xpertRequestResult.getXpertQueryResult().getAdminData(), json["clinical_datas"]);
    getCovariatesJson(_xpertRequestResult.getCovariateValidationResults(), json["covariates"]);
    getTreatmentJson(_xpertRequestResult.getTreatment()->getDosageHistory(), json["treatment"]);

    getSamplesJson(
            _xpertRequestResult.getSampleValidationResults(),
            _xpertRequestResult.getTimeAfterDoseDurations(),
            json["samples"],
            json["treatment"]);

    getAdjustmentsJson(_xpertRequestResult.getAdjustmentData(), json["adjustments"]);
    if (_xpertRequestResult.getAdjustmentData() != nullptr) {
        getTargetsJson(_xpertRequestResult.getAdjustmentData(), json["targets"]);
        getGraphDataJson(_xpertRequestResult, json["graph_data"]);
    }
    getParametersJson(_xpertRequestResult, json["pks"]);

    if (_xpertRequestResult.getAdjustmentData() == nullptr || _xpertRequestResult.getCycleStats().getStats().empty()
        || _xpertRequestResult.getPADAposterioriConcentrations() == nullptr) {
    }
    else {
        getPredictionsJson(_xpertRequestResult, json["predictions"]);
        getComputationCovariatesJson(_xpertRequestResult, json["computation_covariates"]);
        getCompGraphDataJson(_xpertRequestResult, json["compgraph_data"]);
        getPDAAprioriGraphDataJson(_xpertRequestResult, json["pda_apriori_data"]);
        getPDAAposterioriGraphDataJson(_xpertRequestResult, json["pda_aposteriori_data"]);
    }

    std::string jsonDump = json.dump(4);
    if (_xpertRequestResult.getXpertQueryResult().getJsonDumpEnabled()) {
        std::string dumpFolder = _xpertRequestResult.getXpertQueryResult().getOutputPath() + "/jsonDump.json";
        ofstream dumpStream;
        dumpStream.open(dumpFolder);
        dumpStream << jsonDump;
        dumpStream.close();
    }

    if (_isJsTemplate) {
        inja::json jsonJquery;
        jsonJquery["json_dump"] = jsonDump;
        return inja::render(templateStream.str(), jsonJquery);
    }

    return inja::render(templateStream.str(), json);
}

void XpertRequestResultHtmlExport::getHeaderJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _headerJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // TuberXpert report translation
    _headerJson["tuberxpert_report_translation"] = langMgr.translate("tuberxpert_report");

    // Computed on translation
    _headerJson["computed_on_translation"] = langMgr.translate("computed_on");

    // Computation time
    _headerJson["computation_time"] =
            dateTimeToString(_xpertRequestResult.getXpertQueryResult().getComputationTime(), false);
}

void XpertRequestResultHtmlExport::getTranslationReportJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _reportJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    _reportJson["hospital_translate"] = langMgr.translate("hospital");
    _reportJson["report_date_translate"] = langMgr.translate("report_date");
    _reportJson["requesting_practi_translate"] = langMgr.translate("requesting_practitioner");
    _reportJson["page_title"] = langMgr.translate("page_title");

    inja::json justificationJson;
    getJustificationJson(_xpertRequestResult.getJustification(), justificationJson);
    _reportJson["justification_info"].emplace_back(justificationJson);
}

void XpertRequestResultHtmlExport::getDrugIntroJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _introJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Drug translation
    _introJson["drug_translation"] = langMgr.translate("drug");

    // Drug id translation
    _introJson["id_translation"] = langMgr.translate("id");

    // Last dose translation
    _introJson["last_dose_translation"] = langMgr.translate("last_dose");

    // Drug model translation
    _introJson["drug_model_translation"] = langMgr.translate("drug_model");

    // Drug id
    _introJson["drug_id"] = _xpertRequestResult.getXpertRequest().getDrugId();

    // Last dose
    if (_xpertRequestResult.getLastIntake() != nullptr) {
        inja::json lastDoseJson;
        lastDoseJson["dose"] = doubleToString(_xpertRequestResult.getLastIntake()->getDose()) + " "
                               + _xpertRequestResult.getLastIntake()->getUnit().toString();
        lastDoseJson["date"] = dateTimeToString(_xpertRequestResult.getLastIntake()->getEventTime(), true);
        _introJson["last_dose"].emplace_back(lastDoseJson);
    }

    // Drug model
    _introJson["drug_model"] = _xpertRequestResult.getDrugModel()->getDrugModelId();
}

void XpertRequestResultHtmlExport::getContactsJson(const AdminData* _admin, inja::json& _contactsJson) const
{
    // Get the rows and columns header translations
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Contacts translation
    _contactsJson["translation"] = langMgr.translate("contacts");

    // Mandator translation
    _contactsJson["mandator_translation"] = langMgr.translate("mandator");

    // Patient translation
    _contactsJson["patient_translation"] = langMgr.translate("patient");

    // Id translation
    _contactsJson["id_translation"] = langMgr.translate("id");

    // Name translation
    _contactsJson["name_translation"] = langMgr.translate("name");

    // Address translation
    _contactsJson["address_translation"] = langMgr.translate("address");

    // Phome translation
    _contactsJson["phone_translation"] = langMgr.translate("phone");

    // Email translation
    _contactsJson["email_translation"] = langMgr.translate("email");


    // Try to get mandator and patient available information
    if (_admin != nullptr) {
        getPersonJson(_admin->getMandator(), _contactsJson["mandator"]);
        getPersonJson(_admin->getPatient(), _contactsJson["patient"]);
    }
}

void XpertRequestResultHtmlExport::getPersonJson(const FullPersonData* _person, inja::json& _personJson) const
{
    if (_person == nullptr) {
        return;
    }

    // Id
    std::string id = _person->getPerson().getId();
    if (!id.empty()) {
        _personJson["id"] = id;
    }

    // Name
    std::string title = _person->getPerson().getTitle();
    stringstream nameStream;
    if (!title.empty()) {
        nameStream << title << " ";
    }
    nameStream << _person->getPerson().getFirstName() << " " << _person->getPerson().getLastName();
    _personJson["name"] = nameStream.str();

    std::optional<std::string> birthdate = _person->getPerson().getBirthdate();
    if (birthdate.has_value()) {
        _personJson["birthdate"] = birthdate.value();
    }
    else {
        _personJson["birthdate"] = "No given";
    }

    // Address
    getAddressJson(_person->getPerson().getAddress(), _personJson["address"]);

    // Phone
    getPhoneJson(_person->getPerson().getPhone(), _personJson["phone"]);

    // Email
    getEmailJson(_person->getPerson().getEmail(), _personJson["email"]);

    // Institute
    getInstituteJson(_person->getInstitute(), _personJson["institute"]);
}

void XpertRequestResultHtmlExport::getInstituteJson(const InstituteData* _institute, inja::json& _instituteJson) const
{
    if (_institute == nullptr) {
        return;
    }

    // Id
    std::string id = _institute->getId();
    if (!id.empty()) {
        _instituteJson["id"] = id;
    }

    // Name
    _instituteJson["name"] = _institute->getName();

    // Address
    getAddressJson(_institute->getAddress(), _instituteJson["address"]);

    // Phone
    getPhoneJson(_institute->getPhone(), _instituteJson["phone"]);

    // Email
    getEmailJson(_institute->getEmail(), _instituteJson["email"]);
}

void XpertRequestResultHtmlExport::getAddressJson(const AddressData* _address, inja::json& _addressJson) const
{
    if (_address == nullptr) {
        _addressJson = "-";
        return;
    }

    stringstream addressStream;

    // Street
    // Postal code
    // City
    addressStream << _address->getStreet() << "<br>" << _address->getPostalCode() << "<br>" << _address->getCity();

    // State
    std::string state = _address->getState();
    if (!state.empty()) {
        addressStream << "<br>" << state;
    }

    // Country
    std::string country = _address->getCountry();
    if (!country.empty()) {
        addressStream << "<br>" << country;
    }

    _addressJson = addressStream.str();
}

void XpertRequestResultHtmlExport::getPhoneJson(const PhoneData* _phone, inja::json& _phoneJson) const
{
    if (_phone == nullptr) {
        _phoneJson = "-";
        return;
    }

    LanguageManager& langMgr = LanguageManager::getInstance();
    stringstream phoneStream;

    // Number
    phoneStream << _phone->getNumber();

    // Type
    std::string type = _phone->getType();
    if (!type.empty()) {
        phoneStream << " (" << langMgr.translate(type) << ")";
    }

    _phoneJson = phoneStream.str();
}

void XpertRequestResultHtmlExport::getEmailJson(const EmailData* _email, inja::json& _emailJson) const
{
    if (_email == nullptr) {
        _emailJson = "-";
        return;
    }

    LanguageManager& langMgr = LanguageManager::getInstance();
    stringstream emailStream;

    // Address
    emailStream << _email->getAddress();

    // Type
    std::string type = _email->getType();
    if (!type.empty()) {
        emailStream << " (" << langMgr.translate(type) << ")";
    }

    _emailJson = emailStream.str();
}

void XpertRequestResultHtmlExport::getClinicalDatasJson(const AdminData* _admin, inja::json& _clinicalDatasJson) const
{

    LanguageManager& lm = LanguageManager::getInstance();

    // Clinical data translation
    _clinicalDatasJson["clinical_data_translation"] = lm.translate("clinical_data");

    // None translation
    _clinicalDatasJson["none_translation"] = lm.translate("none");

    // If there are clinical data
    if (_admin != nullptr && _admin->getClinicalDatas() != nullptr) {

        // For each clinical data
        for (const auto& clinicalDataIt : _admin->getClinicalDatas()->getData()) {

            // Create json value
            inja::json clinicalDataJson;
            clinicalDataJson["key"] = keyToPhrase(clinicalDataIt.first);
            clinicalDataJson["value"] = clinicalDataIt.second;

            // Add the row to the row list
            _clinicalDatasJson["clinical_datas"].emplace_back(clinicalDataJson);
        }
    }
}

void XpertRequestResultHtmlExport::getCovariatesJson(
        const vector<CovariateValidationResult>& _covariateResults, inja::json& _covariatesJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Clinical data translation
    _covariatesJson["translation"] = langMgr.translate("covariates");

    // None translation
    _covariatesJson["none_translation"] = langMgr.translate("none");

    // Value translation
    _covariatesJson["value_translation"] = langMgr.translate("value");

    // Date translation
    _covariatesJson["date_translation"] = langMgr.translate("date");

    // For each covariate result
    for (const CovariateValidationResult& cvr : _covariateResults) {

        inja::json covariateJson;

        // Get the covariate name translated or english
        covariateJson["name"] = getStringWithEnglishFallback(
                cvr.getSource()->getName(), m_xpertRequestResultInUse->getXpertRequest().getOutputLang());

        // Get the covariate description
        covariateJson["desc"] = getStringWithEnglishFallback(
                cvr.getSource()->getDescription(), m_xpertRequestResultInUse->getXpertRequest().getOutputLang());

        // Get the covariate warning
        getWarningJson(cvr, covariateJson);

        // Get the covariate value.
        std::string value = beautifyString(cvr.getValue(), cvr.getDataType(), cvr.getSource()->getId());

        std::string unit = cvr.getUnit().toString();

        // Format the value + unit + source
        stringstream valueStream;

        if (cvr.getSource()->getId() == "age" && cvr.getPatientCovariate() != nullptr) {
            valueStream << static_cast<int>(getAgeIn(
                    cvr.getSource()->getType(),
                    cvr.getPatientCovariate()->getValueAsDate(),
                    m_xpertRequestResultInUse->getXpertQueryResult().getComputationTime()));
        }
        else {
            valueStream << value;
        }

        if (unit != "-" && !unit.empty()) {
            valueStream << " " << unit;
        }

        valueStream << " (" << langMgr.translate("source_" + covariateTypeToString(cvr.getType())) << ")";

        covariateJson["value"] = valueStream.str();

        // Get the covariate date if it is a patient covariate
        if (cvr.getPatientCovariate() != nullptr) {
            covariateJson["date"] = dateTimeToString(cvr.getPatientCovariate()->getEventTime(), false);
        }

        _covariatesJson["covariates"].emplace_back(covariateJson);
    }
}

void XpertRequestResultHtmlExport::getTreatmentJson(
        const Core::DosageHistory& _history, inja::json& _treatmentJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Treatment translation
    _treatmentJson["translation"] = langMgr.translate("treatment");

    // None translation
    _treatmentJson["none_translation"] = langMgr.translate("none");

    // From translation
    _treatmentJson["from_translation"] = langMgr.translate("from");

    // To translation
    _treatmentJson["to_translation"] = langMgr.translate("to");

    // Type translation
    _treatmentJson["type_translation"] = langMgr.translate("type");

    // Posologie translation
    _treatmentJson["dosage_translation"] = langMgr.translate("dosage");

    // Formulation and route translation
    _treatmentJson["formulation_translation"] = langMgr.translate("formulation");

    // Export the dosage time ranges
    for (const auto& dosageTimeRange : _history.getDosageTimeRanges()) {
        inja::json timeRangeJson;
        getTimeRangeJson(dosageTimeRange.get(), timeRangeJson);
        _treatmentJson["dosage_time_ranges"].emplace_back(timeRangeJson);
    }
}

void XpertRequestResultHtmlExport::getTimeRangeJson(
        const Core::DosageTimeRange* _timeRange, inja::json& _dosageTimeRangeJson) const
{
    // Set date from value
    _dosageTimeRangeJson["date_from"] = dateTimeToString(_timeRange->getStartDate());

    // Set date to value
    _dosageTimeRangeJson["date_to"] = dateTimeToString(_timeRange->getEndDate());

    getAbstractDosageJson(*_timeRange->getDosage(), _dosageTimeRangeJson, "");
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TRY_GET_JSON(Type)                                                                                         \
    if (dynamic_cast<const Tucuxi::Core::Type*>(&_dosage)) {                                                       \
        return getDosageJson(                                                                                      \
                *dynamic_cast<const Tucuxi::Core::Type*>(&_dosage), _dosageTimeRangeJson, _dosageIndicationChain); \
    }

void XpertRequestResultHtmlExport::getAbstractDosageJson(
        const Core::Dosage& _dosage, inja::json& _dosageTimeRangeJson, const std::string& _dosageIndicationChain) const
{
    // The calls order is important here.
    // First start with the subclasses, else it won't work.
    TRY_GET_JSON(SimpleDoseList);
    TRY_GET_JSON(SingleDoseAtTimeList);
    TRY_GET_JSON(WeeklyDose);
    TRY_GET_JSON(DailyDose);
    TRY_GET_JSON(LastingDose);
    TRY_GET_JSON(ParallelDosageSequence);
    TRY_GET_JSON(DosageLoop);
    TRY_GET_JSON(DosageSteadyState);
    TRY_GET_JSON(DosageRepeat);
    TRY_GET_JSON(DosageSequence);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::SingleDoseAtTimeList& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    auto dosageList = _dosage.getDosageList();
    auto formulationAndRoute = dosageList.back().getFormulationAndRoute();
    auto unit = dosageList.back().getDoseUnit();

    // Get the route.
    static map<Tucuxi::Core::AdministrationRoute, std::string> routes = {
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

    auto it = routes.find(formulationAndRoute.getAdministrationRoute());
    std::string adminRoute;
    if (it == routes.end()) {
        adminRoute = "undefined";
    }
    else {
        adminRoute = it->second;
    }

    // Get the last dosage.
    auto singleDosageAtTime = dosageList.back();
    stringstream dosageStream;
    dosageStream << "" << singleDosageAtTime.getDoseValue() << " " << unit.toString() << ""
                 << " (" << adminRoute << ")";

    std::string newDosageIndicationChain = prefixDosage(dosageStream.str(), _dosageIndicationChain);

    // Add the dosage of the single dose
    inja::json singleDoseJson;
    singleDoseJson["dosage"] = newDosageIndicationChain;

    // Add the potential warning
    auto singleDoseIt = m_xpertRequestResultInUse->getDoseValidationResults().find(&_dosage);
    if (singleDoseIt != m_xpertRequestResultInUse->getDoseValidationResults().end()) {
        getWarningJson(singleDoseIt->second, singleDoseJson);
    }

    // Add the single dose to the single doses list of the time range.
    _dosageTimeRangeJson["single_doses"].emplace_back(singleDoseJson);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::SimpleDoseList& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    auto dosageList = _dosage.getDosageList();
    auto formulationAndRoute = _dosage.getFormulationAndRoute();
    auto unit = _dosage.getDoseUnit();

    // Get the route.
    static map<Tucuxi::Core::AdministrationRoute, std::string> routes = {
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

    auto it = routes.find(formulationAndRoute.getAdministrationRoute());
    std::string adminRoute;
    if (it == routes.end()) {
        adminRoute = "undefined";
    }
    else {
        adminRoute = it->second;
    }

    // Get the last dosage.
    auto simpleDosage = dosageList.back();
    stringstream dosageStream;
    dosageStream << "" << simpleDosage.getDoseValue() << " " << unit.toString() << ""
                 << " (" << adminRoute << ")";

    std::string newDosageIndicationChain = prefixDosage(dosageStream.str(), _dosageIndicationChain);

    // Add the dosage of the single dose
    inja::json singleDoseJson;
    singleDoseJson["dosage"] = newDosageIndicationChain;

    // Add the potential warning
    auto singleDoseIt = m_xpertRequestResultInUse->getDoseValidationResults().find(&_dosage);
    if (singleDoseIt != m_xpertRequestResultInUse->getDoseValidationResults().end()) {
        getWarningJson(singleDoseIt->second, singleDoseJson);
    }

    // Add the single dose to the single doses list of the time range.
    _dosageTimeRangeJson["single_doses"].emplace_back(singleDoseJson);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::DosageLoop& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Set the type of the dosage time range
    LanguageManager& langMgr = LanguageManager::getInstance();
    _dosageTimeRangeJson["type"] = langMgr.translate("continually");

    // Keep digging into the dosage tree
    getAbstractDosageJson(*_dosage.getDosage(), _dosageTimeRangeJson, _dosageIndicationChain);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::DosageSteadyState& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Set the type of the dosage time range.
    LanguageManager& langMgr = LanguageManager::getInstance();

    stringstream typeStream;
    typeStream << langMgr.translate("at_steady_state") << " " << dateTimeToString(_dosage.getLastDoseTime());
    _dosageTimeRangeJson["type"] = typeStream.str();

    // Keep digging into the dosage tree.
    getAbstractDosageJson(*_dosage.getDosage(), _dosageTimeRangeJson, _dosageIndicationChain);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::DosageRepeat& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Add indication in the dosage indication chain
    LanguageManager& langMgr = LanguageManager::getInstance();

    stringstream dosageStream;
    dosageStream << _dosage.getNbTimes() << " " << langMgr.translate("times");
    std::string newDosageIndicationChain = prefixDosage(dosageStream.str(), _dosageIndicationChain);

    // Keep digging into the dosage tree.
    getAbstractDosageJson(*_dosage.getDosage(), _dosageTimeRangeJson, newDosageIndicationChain);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::DosageSequence& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Iterate over the dosages.
    for (const unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosage.getDosageList()) {
        getAbstractDosageJson(*dosage, _dosageTimeRangeJson, _dosageIndicationChain);
    }
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::ParallelDosageSequence& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    auto timeOffsetIt = _dosage.getOffsetsList().begin();

    // For each dosage, add the offset in the dosage indication chain.
    for (const unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosage.getDosageList()) {

        stringstream offsetStream;
        offsetStream << langMgr.translate("offset") << " "
                     << timeOfDayToString(TimeOfDay::buildUnnormalized(*timeOffsetIt));
        std::string newDosageIndicationChain = prefixDosage(offsetStream.str(), _dosageIndicationChain);

        getAbstractDosageJson(*dosage, _dosageTimeRangeJson, newDosageIndicationChain);
        timeOffsetIt++;
    }
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::LastingDose& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Add the indication in the dosage indication chain
    LanguageManager& langMgr = LanguageManager::getInstance();

    stringstream dosageStream;
    dosageStream << langMgr.translate("interval") << " "
                 << timeOfDayToString(TimeOfDay::buildUnnormalized(_dosage.getTimeStep()));
    std::string newDosageIndicationChain = prefixDosage(dosageStream.str(), _dosageIndicationChain);

    // Export the single dose
    getSingleDoseJson(_dosage, _dosageTimeRangeJson, newDosageIndicationChain);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::DailyDose& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Add the indication in the dosage indication chain
    LanguageManager& langMgr = LanguageManager::getInstance();

    stringstream dosageStream;
    dosageStream << langMgr.translate("daily_at") << " " << timeOfDayToString(_dosage.getTimeOfDay());
    std::string newDosageIndicationChain = prefixDosage(dosageStream.str(), _dosageIndicationChain);

    // Export the single dose
    getSingleDoseJson(_dosage, _dosageTimeRangeJson, newDosageIndicationChain);
}

void XpertRequestResultHtmlExport::getDosageJson(
        const Core::WeeklyDose& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Add the indication in the dosage indication chain
    LanguageManager& langMgr = LanguageManager::getInstance();

    stringstream dosageStream;
    dosageStream << langMgr.translate("every") << " "
                 << langMgr.translate("day_" + std::to_string(_dosage.getDayOfWeek().iso_encoding())) << " "
                 << langMgr.translate("at") << " " << timeOfDayToString(_dosage.getTimeOfDay());
    std::string newDosageIndicationChain = prefixDosage(dosageStream.str(), _dosageIndicationChain);

    // Export the single dose
    getSingleDoseJson(_dosage, _dosageTimeRangeJson, newDosageIndicationChain);
}

void XpertRequestResultHtmlExport::getSingleDoseJson(
        const Core::SingleDose& _dosage,
        inja::json& _dosageTimeRangeJson,
        const std::string& _dosageIndicationChain) const
{
    // Get the route
    LanguageManager& langMgr = LanguageManager::getInstance();
    static map<Tucuxi::Core::AdministrationRoute, std::string> routes = {
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

    // Get the dosage
    stringstream dosageStream;
    dosageStream << doubleToString(_dosage.getDose()) << " " << _dosage.getDoseUnit().toString();

    // If the route is defined, add it to the dosage
    if (it != routes.end() && it->second != "undefined") {
        dosageStream << " (" << langMgr.translate(it->second) << ")";
    }

    std::string newDosageIndicationChain = prefixDosage(dosageStream.str(), _dosageIndicationChain);

    // Add the dosage of the single dose
    inja::json singleDoseJson;
    singleDoseJson["dosage"] = newDosageIndicationChain;

    // Add the potential warning
    auto singleDoseIt = m_xpertRequestResultInUse->getDoseValidationResults().find(&_dosage);
    if (singleDoseIt != m_xpertRequestResultInUse->getDoseValidationResults().end()) {
        getWarningJson(singleDoseIt->second, singleDoseJson);
    }

    // Add the single dose to the single doses list of the time range.
    _dosageTimeRangeJson["single_doses"].emplace_back(singleDoseJson);
}

std::optional<std::string> XpertRequestResultHtmlExport::get_dosage(const inja::json& _j) const
{
    if (!_j.contains("dosage_time_ranges") || !_j["dosage_time_ranges"].is_array()) {
        return std::nullopt;
    }
    if (_j["dosage_time_ranges"].empty()) {
        return std::nullopt;
    }

    const auto& firstRange = _j["dosage_time_ranges"][0];

    if (!firstRange.contains("single_doses") || !firstRange["single_doses"].is_array()) {
        return std::nullopt;
    }
    if (firstRange["single_doses"].empty()) {
        return std::nullopt;
    }
    const auto& firstDose = firstRange["single_doses"][0];
    if (!firstDose.contains("dosage") || !firstDose["dosage"].is_string()) {
        return std::nullopt;
    }

    return firstDose["dosage"].get<std::string>();
}

void XpertRequestResultHtmlExport::getSamplesJson(
        const vector<SampleValidationResult>& _sampleResults,
        const std::vector<Duration>& _timeAfterDoseDurations,
        inja::json& _samplesJson,
        inja::json& _treatmentJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Get samples translation
    _samplesJson["translation"] = langMgr.translate("samples");

    // Get none translation
    _samplesJson["none_translation"] = langMgr.translate("none");

    // Get measure translation
    _samplesJson["measure_translation"] = langMgr.translate("measure");

    // Get date translation
    _samplesJson["date_translation"] = langMgr.translate("date");

    // Get percentile translation
    _samplesJson["percentile_translation"] = langMgr.translate("percentile");

    for (size_t i = 0; i < _sampleResults.size(); ++i) {
        inja::json sampleJson;
        const auto& sampleValidationResult = _sampleResults.at(i);

        // Get the sample date
        sampleJson["date"] = dateTimeToString(sampleValidationResult.getSource()->getDate());

        // Get the sample measure
        stringstream valueStream;
        valueStream << sampleValidationResult.getSource()->getValue() << " "
                    << sampleValidationResult.getSource()->getUnit().toString();
        sampleJson["measure"] = valueStream.str();

        // Get the percentile
        sampleJson["percentile"] = std::to_string(sampleValidationResult.getGroupNumberOver99Percentile());

        // Get the warning
        getWarningJson(sampleValidationResult, sampleJson);

        // Get the warning level
        sampleJson["warning_level"] = warningLevelToString(sampleValidationResult.getWarningLevel());

        if (!_timeAfterDoseDurations.empty()) {
            stringstream tadStream;
            tadStream.precision(2);
            tadStream << _timeAfterDoseDurations.at(i).toHours() << " h";
            sampleJson["tad"] = tadStream.str();
        }

        if (i + 1 == _sampleResults.size()) {
            std::string dosageString;
            auto dosage = get_dosage(_treatmentJson);

            if (dosage) {
                dosageString = *dosage;
            }
            else {
                // Valeur absente ou type incorrect
            }
            sampleJson["warning_sentence"] = SampleValidationResult::computePercentileSentence(
                    sampleValidationResult.getGroupNumberOver99Percentile(),
                    sampleValidationResult.getSource()->getValue(),
                    sampleValidationResult.getSource()->getUnit().toString(),
                    dosageString,
                    sampleValidationResult.isAscending());
        }

        _samplesJson["samples"].emplace_back(sampleJson);
    }
}

void XpertRequestResultHtmlExport::getAdjustmentsJson(
        const Core::AdjustmentData* _adjustmentData, inja::json& _adjustmentsJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Get adjustments translation
    _adjustmentsJson["translation"] = langMgr.translate("adjustments");

    // Get intro phrase translation
    _adjustmentsJson["intro_phrase_translation"] = langMgr.translate("intro_phrase");

    // Get per interval translation
    _adjustmentsJson["per_interval_translation"] = langMgr.translate("per_interval");

    // Get displayed adjustments translation
    _adjustmentsJson["displayed_adjustments_translation"] = langMgr.translate("displayed_adjustments");

    // Get displayed adjustments translation
    _adjustmentsJson["displayed_adjustment_translation"] = langMgr.translate("displayed_adjustment");

    // Get score translation
    _adjustmentsJson["score_translation"] = langMgr.translate("score");

    // Get from translation
    _adjustmentsJson["from_translation"] = langMgr.translate("from");

    // Get to translation
    _adjustmentsJson["to_translation"] = langMgr.translate("to");

    // Get dosage translation
    _adjustmentsJson["dosage_translation"] = langMgr.translate("dosage");

    // Get suggestion translation
    _adjustmentsJson["adjustment_suggested_translation"] = langMgr.translate("adjustment_suggested");

    // Get suggestion phrase translation
    _adjustmentsJson["suggestion_phrase_translation"] = langMgr.translate("suggestion_phrase");

    if (_adjustmentData == nullptr) {
        _adjustmentsJson["no_adjustment_phrase"] = langMgr.translate("no_adjustments_phrase");
        return;
    }

    // For each adjustment
    for (const Core::DosageAdjustment& adj : _adjustmentData->getAdjustments()) {
        inja::json adjustmentJson;

        // Get adjustment score
        adjustmentJson["score"] = doubleToString(adj.getGlobalScore());

        // Export the dosage time ranges
        for (const unique_ptr<Core::DosageTimeRange>& dosageTimeRange : adj.m_history.getDosageTimeRanges()) {
            inja::json timeRangeJson;
            getTimeRangeJson(dosageTimeRange.get(), timeRangeJson);
            adjustmentJson["dosage_time_ranges"].emplace_back(timeRangeJson);
        }

        _adjustmentsJson["adjustments"].emplace_back(adjustmentJson);
    }
}

void XpertRequestResultHtmlExport::getJustificationJson(
        const Justification& _justification, inja::json& _justificationJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    _justificationJson["before_title"] = langMgr.translate("justification_before_title");
    _justificationJson["new_title"] = langMgr.translate("justification_new_title");

    if (_justification.getJustificationType() == JustificationType::SIMPLE) {
        _justificationJson["type"] = "simple";

        std::stringstream ss;
        ss << _justification.getFirstDoseText() << "(<strong>" << langMgr.translate("starting_from") << "</strong>"
           << dateTimeToString(_justification.getFirstDoseDate()) << ")";

        _justificationJson["dosage_sentence"] = ss.str();
    }
    else if (_justification.getJustificationType() == JustificationType::DOUBLE) {
        _justificationJson["type"] = "double";

        if (_justification.getFirstDoseValue() == 0) {
            _justificationJson["before_dosage_sentence"] = langMgr.translate("justification_rest_period");
        }
        else {
            std::stringstream ss;
            ss << langMgr.translate("justification_adaptative_dose") << ":<strong>"
               << dateTimeToString(_justification.getFirstDoseDate())
               << "</strong>: " << _justification.getFirstDoseText();
            _justificationJson["before_dosage_sentence"] = ss.str();
        }

        std::stringstream ss;
        ss << _justification.getSecondDoseText() << "(<strong>" << langMgr.translate("starting_from") << "</strong>"
           << dateTimeToString(_justification.getSecondDoseDate()) << ")";

        _justificationJson["dosage_sentence"] = ss.str();
    }

    switch (_justification.getJustificationExposureSign()) {
    case JustificationExposureSign::ABOVE:
        _justificationJson["justification_exposure_sentence"] = langMgr.translate("above_exposure");
        break;
    case JustificationExposureSign::BELOW:
        _justificationJson["justification_exposure_sentence"] = langMgr.translate("bellow_exposure");
        break;
    case JustificationExposureSign::EQUAL:
        _justificationJson["justification_exposure_sentence"] = langMgr.translate("equal_exposure");
        break;
    default:
        _justificationJson["justification_exposure_sentence"] = "";
    }

    switch (_justification.getJustificationInterval()) {
    case JustificationInterval::HIGHER:
        _justificationJson["justification_interval_sentence"] = langMgr.translate("higher_interval");
        break;
    case JustificationInterval::EQUAL:
        _justificationJson["justification_interval_sentence"] = langMgr.translate("equal_interval");
        break;
    case JustificationInterval::LOWER:
        _justificationJson["justification_interval_sentence"] = langMgr.translate("lower_interval");
        break;
    default:
        _justificationJson["justification_interval_sentence"] = "";
        break;
    }

    switch (_justification.getJustificationDoseSign()) {
    case JustificationDoseSign::DECREASE:
        _justificationJson["justification_dosage_sentence"] = langMgr.translate("decrease_dosage");
        _justificationJson["justification_sign"] = "decrease";
        break;
    case JustificationDoseSign::INCREASE:
        _justificationJson["justification_dosage_sentence"] = langMgr.translate("increase_dosage");
        _justificationJson["justification_sign"] = "increase";
        break;
    case JustificationDoseSign::EQUAL:
        _justificationJson["justification_dosage_sentence"] = langMgr.translate("equal_dosage");
        _justificationJson["justification_sign"] = "equal";
        break;
    case JustificationDoseSign::NEW:
        _justificationJson["justification_dosage_sentence"] = langMgr.translate("new_dosage");
        _justificationJson["justification_sign"] = "new";
        break;
    default:
        _justificationJson["justification_exposure_sentence"] = "";
    }
}

void XpertRequestResultHtmlExport::getTargetsJson(
        const Core::AdjustmentData* _adjustmentData, inja::json& _targetsJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Targets phrase translation
    _targetsJson["phrase_translation"] = langMgr.translate("targets_phrase");

    // For each target in the best suggestion
    for (const auto& target : _adjustmentData->getAdjustments()[0].m_targetsEvaluation) {
        inja::json targetJson;

        // Get the target type translation
        static map<Core::TargetType, std::string> types = {
                {Core::TargetType::UnknownTarget, "unknown"},
                {Core::TargetType::Residual, "residual"},
                {Core::TargetType::Peak, "peak"},
                {Core::TargetType::Mean, "mean"},
                {Core::TargetType::Auc, "auc"},
                {Core::TargetType::Auc24, "auc24"},
                {Core::TargetType::CumulativeAuc, "cumulative_auc"},
                {Core::TargetType::AucOverMic, "auc_over_mic"},
                {Core::TargetType::Auc24OverMic, "auc24_over_mic"},
                {Core::TargetType::TimeOverMic, "time_over_mic"},
                {Core::TargetType::AucDividedByMic, "auc_divided_by_mic"},
                {Core::TargetType::Auc24DividedByMic, "auc24_divided_by_mic"},
                {Core::TargetType::PeakDividedByMic, "peak_divided_by_mic"},
                {Core::TargetType::ResidualDividedByMic, "residual_divided_by_mic"},
                {Core::TargetType::FractionTimeOverMic, "fraction_time_over_mic"}};

        targetJson["type"] = langMgr.translate(types.at(target.getTargetType()));

        // Get the value (unit)
        stringstream valueStream;
        valueStream << doubleToString(target.getValue()) << " (" << target.getUnit().toString() << ")";
        targetJson["value"] = valueStream.str();

        // Get the score
        targetJson["score"] = doubleToString(target.getScore());

        // --------------- /!\ TO ADD WHEN ADJUSTMENT DATA HAS TARGET INEFFICACY AND TOXICITY /!\ -----------

        //        // Get the bounds
        //        stringstream boundsStream;
        //        boundsStream << langMgr.translate("inefficacy") << ": " << int(target.getTarget().getInefficacyAlarm()) << " / "
        //                     << "<b>"
        //                     << langMgr.translate("min") << ": " << int(target.getTarget().getValueMin()) << " / "
        //                     << langMgr.translate("best") << ": " << int(target.getTarget().getValueBest()) << " / "
        //                     << langMgr.translate("max") << ": " << int(target.getTarget().getValueMax()) << " / "
        //                     << "</b>"
        //                     << langMgr.translate("toxicity") << ": " << int(target.getTarget().getToxicityAlarm());

        // --------------- /!\                      END TO ADD                                /!\ -----------


        // --------------- /!\ TO REMOVE WHEN ADJUSTMENT DATA HAS TARGET INEFFICACY AND TOXICITY /!\ -----------

        // Get the active moieties and find the active moiety of the target
        const Core::ActiveMoieties& activeMoieties = m_xpertRequestResultInUse->getDrugModel()->getActiveMoieties();
        auto activeMoityIt =
                find_if(activeMoieties.begin(),
                        activeMoieties.end(),
                        [&target](const unique_ptr<Core::ActiveMoiety>& _activeMoiety) {
                            return _activeMoiety->getActiveMoietyId() == target.getTarget().getActiveMoietyId();
                        });

        // Get the target definitions of the active moiety and search if there is a target definition that
        // matches the current target type.
        const Core::TargetDefinitions& modelTargets = (*activeMoityIt)->getTargetDefinitions();
        auto targetDefinitionIt =
                find_if(modelTargets.begin(),
                        modelTargets.end(),
                        [&target](const unique_ptr<Core::TargetDefinition>& _targetDefinition) {
                            return _targetDefinition->getTargetType() == target.getTargetType();
                        });

        // Display the values of the definition if there is one, otherwise -1.
        std::string inefficacyAlarm =
                targetDefinitionIt != modelTargets.end()
                        ? std::to_string(static_cast<int>((*targetDefinitionIt)->getInefficacyAlarm().getValue()))
                        : langMgr.translate("unknown");
        std::string toxicityAlarm =
                targetDefinitionIt != modelTargets.end()
                        ? std::to_string(static_cast<int>((*targetDefinitionIt)->getToxicityAlarm().getValue()))
                        : langMgr.translate("unknown");


        // Get the bounds
        stringstream boundsStream;
        boundsStream << langMgr.translate("inefficacy") << ": " << inefficacyAlarm << " / "
                     << "<b>" << langMgr.translate("min") << ": " << static_cast<int>(target.getTarget().getValueMin())
                     << " / " << langMgr.translate("best") << ": "
                     << static_cast<int>(target.getTarget().getValueBest()) << " / " << langMgr.translate("max") << ": "
                     << static_cast<int>(target.getTarget().getValueMax()) << " / "
                     << "</b>" << langMgr.translate("toxicity") << ": " << toxicityAlarm;

        // --------------- /!\                      END TO REMOVE                                 /!\ -----------

        targetJson["bounds"] = boundsStream.str();

        _targetsJson["targets"].emplace_back(targetJson);
    }
}

void XpertRequestResultHtmlExport::getParametersJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _pksJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Pharmacokinetic parameters translation
    _pksJson["translation"] = langMgr.translate("pharmacokinetic_parameters");

    // Typical patient translation
    _pksJson["typical_patient_translation"] = langMgr.translate("typical_patient");

    // A priori phrase translation
    _pksJson["a_priori_translation"] = langMgr.translate("a_priori");

    // A posteriori phrase translation
    _pksJson["a_posteriori_translation"] = langMgr.translate("a_posteriori");

    // For each type series (0: typical patient, 1: a priori, 2: a posteriori)
    for (size_t i = 0; i < _xpertRequestResult.getParameters().size(); i++) {

        // For each parameter.
        // We take advantage of the fact that the parameters are always in the same order
        // in the 3 vectors. Thus, we can easily add the a priori and a posteriori values.
        for (size_t j = 0; j < _xpertRequestResult.getParameters()[i].size(); ++j) {

            const Core::ParameterValue& parameterValue = _xpertRequestResult.getParameters()[i][j];

            // Insert the parameter value depending on the type.
            switch (i) {

            // Typical patient
            case 0: {
                inja::json parameterJson;
                parameterJson["name"] = parameterValue.m_parameterId;
                parameterJson["typical_patient"] = doubleToString(parameterValue.m_value);
                _pksJson["pks"].emplace_back(parameterJson);
                break;
            }

            // A priori
            case 1: {
                _pksJson["pks"][j]["a_priori"] = doubleToString(parameterValue.m_value);
                break;
            }

            // A posteriori
            case 2: {
                _pksJson["pks"][j]["a_posteriori"] = doubleToString(parameterValue.m_value);
                break;
            }

            default: {
                // Should never happen, by construction
            }
            }
        }
    }
}

void XpertRequestResultHtmlExport::getPredictionsJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _predictionsJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Predictions translation
    _predictionsJson["translation"] = langMgr.translate("predictions");

    // Extrapolated steady state auc24 translation
    _predictionsJson["extrapolated_steady_state_auc24_translation"] =
            langMgr.translate("extrapolated_steady_state_auc24");

    // Steady state peak translation
    _predictionsJson["steady_state_peak_translation"] = langMgr.translate("steady_state_peak");

    // Steady state residual translation
    _predictionsJson["steady_state_residual_translation"] = langMgr.translate("steady_state_residual");

    double auc24 = -1.0;
    double peak = -1.0;
    double residual = -1.0;
    Tucuxi::Common::DateTime date;

    _xpertRequestResult.getCycleStats().getStatistic(0, Tucuxi::Core::CycleStatisticType::AUC24).getValue(date, auc24);
    _xpertRequestResult.getCycleStats().getStatistic(0, Tucuxi::Core::CycleStatisticType::Peak).getValue(date, peak);
    _xpertRequestResult.getCycleStats()
            .getStatistic(0, Tucuxi::Core::CycleStatisticType::Residual)
            .getValue(date, residual);

    // Extrapolated steady state auc24
    _predictionsJson["extrapolated_steady_state_auc24"] = doubleToString(auc24) + " µg*h/l";

    // Steady state peak
    _predictionsJson["steady_state_peak"] = doubleToString(peak) + " µg/l";

    // Steady state residual
    _predictionsJson["steady_state_residual"] = doubleToString(residual) + " µg/l";
}

void XpertRequestResultHtmlExport::getComputationCovariatesJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _computationCovariatesJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    // Computation covariates translation
    _computationCovariatesJson["translation"] = langMgr.translate("computation_covariates");

    // Covariate ID translation
    _computationCovariatesJson["covariate_translation"] = langMgr.translate("covariate");

    // Steady state peak translation
    _computationCovariatesJson["value_translation"] = langMgr.translate("value");

    // Extract the covariates used during the computation
    const vector<Core::CovariateValue>& computationCovariates =
            _xpertRequestResult.getAdjustmentData()->getAdjustments().front().getData().front().m_covariates;
    for (const Core::CovariateValue& covariateValue : computationCovariates) {

        inja::json computationCovariateJson;

        std::string covariateId = covariateValue.m_covariateId;

        // Find the covariate definition in the drug model
        const Core::CovariateDefinitions& covariateDefinitions = _xpertRequestResult.getDrugModel()->getCovariates();

        auto covariteDefinitionIt =
                find_if(covariateDefinitions.begin(),
                        covariateDefinitions.end(),
                        [&covariateId](const unique_ptr<Core::CovariateDefinition>& _covariateDefinition) {
                            return _covariateDefinition->getId() == covariateId;
                        });

        // Get the name
        computationCovariateJson["name"] = getStringWithEnglishFallback(
                (*covariteDefinitionIt)->getName(), _xpertRequestResult.getXpertRequest().getOutputLang());

        // Get the value
        computationCovariateJson["value"] = beautifyString(
                doubleToString(covariateValue.m_value),
                (*covariteDefinitionIt)->getDataType(),
                (*covariteDefinitionIt)->getId());

        _computationCovariatesJson["computation_covariates"].emplace_back(computationCovariateJson);
    }
}

void XpertRequestResultHtmlExport::getGraphDataJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _graphDataJson) const
{
    // Get the min x date
    _graphDataJson["adjustment_date"] =
            "'" + Common::Utils::varToString(_xpertRequestResult.getAdjustmentTrait()->getAdjustmentTime()) + "'";

    // Get the max x date
    _graphDataJson["end_date"] =
            "'" + Common::Utils::varToString(_xpertRequestResult.getAdjustmentTrait()->getEnd()) + "'";

    // For each target in the best suggestion
    for (const Core::TargetEvaluationResult& target :
         _xpertRequestResult.getAdjustmentData()->getAdjustments()[0].m_targetsEvaluation) {

        inja::json targetJson;

        // Add target type
        targetJson["type"] = Core::toString(target.getTargetType());

        // Add target min
        targetJson["min"] = doubleToString(target.getTarget().getValueMin());

        // Add target best
        targetJson["best"] = doubleToString(target.getTarget().getValueBest());

        // Add target max
        targetJson["max"] = doubleToString(target.getTarget().getValueMax());


        _graphDataJson["targets"].emplace_back(targetJson);
    }

    // Create the js array of adjustments
    const vector<Core::DosageAdjustment>& adjustments = _xpertRequestResult.getAdjustmentData()->getAdjustments();

    // For each adjustment
    for (const auto& adjustment : adjustments) {

        inja::json adjustmentJson;

        // For each cycle data
        for (const auto& cycleData : adjustment.getData()) {

            inja::json cycleDataJson;


            // Add the cycle data start time
            cycleDataJson["start"] = Common::Utils::varToString(cycleData.m_start);

            // Add the cycle data offsets
            stringstream timesStream;
            for (size_t i = 0; i < cycleData.m_times[0].size(); i++) {
                timesStream << std::to_string(cycleData.m_times[0][i]);
                if (i != cycleData.m_times[0].size() - 1) {
                    timesStream << ',';
                }
            }
            cycleDataJson["times"] = timesStream.str();

            // Add the cycle data values
            stringstream valuesStream;
            for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                valuesStream << std::to_string(cycleData.m_concentrations[0][i]);
                if (i != cycleData.m_concentrations[0].size() - 1) {
                    valuesStream << ',';
                }
            }
            cycleDataJson["values"] = valuesStream.str();


            adjustmentJson["cycles"].emplace_back(cycleDataJson);
        }

        _graphDataJson["adjustments"].emplace_back(adjustmentJson);
    }
}

void XpertRequestResultHtmlExport::getCompGraphDataJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _graphCompDataJson) const
{
    LanguageManager& langMgr = LanguageManager::getInstance();

    static map<Core::TargetType, std::string> types = {
            {Core::TargetType::UnknownTarget, "unknown"},
            {Core::TargetType::Residual, "residual"},
            {Core::TargetType::Peak, "peak"},
            {Core::TargetType::Mean, "mean"},
            {Core::TargetType::Auc, "auc"},
            {Core::TargetType::Auc24, "auc24"},
            {Core::TargetType::CumulativeAuc, "cumulative_auc"},
            {Core::TargetType::AucOverMic, "auc_over_mic"},
            {Core::TargetType::Auc24OverMic, "auc24_over_mic"},
            {Core::TargetType::TimeOverMic, "time_over_mic"},
            {Core::TargetType::AucDividedByMic, "auc_divided_by_mic"},
            {Core::TargetType::Auc24DividedByMic, "auc24_divided_by_mic"},
            {Core::TargetType::PeakDividedByMic, "peak_divided_by_mic"},
            {Core::TargetType::ResidualDividedByMic, "residual_divided_by_mic"},
            {Core::TargetType::FractionTimeOverMic, "fraction_time_over_mic"}};

    size_t size = _xpertRequestResult.getAdjustmentData()->getAdjustments()[0].m_targetsEvaluation.size();
    size_t sizeCurrent =
            _xpertRequestResult.getAdjustmentData()->getCurrentDosageWithScore().m_targetsEvaluation.size();

    for (size_t i = 0; i < size; ++i) {
        inja::json compGraphJson;
        const auto targetAfter = _xpertRequestResult.getAdjustmentData()->getAdjustments()[0].m_targetsEvaluation.at(i);

        if (i < sizeCurrent) {
            const auto targetBefore =
                    _xpertRequestResult.getAdjustmentData()->getCurrentDosageWithScore().m_targetsEvaluation.at(i);
            compGraphJson["valueBefore"] = targetBefore.getValue();
        }
        else {
            compGraphJson["valueBefore"] = "";
        }

        compGraphJson["targetName"] = langMgr.translate(types.at(targetAfter.getTargetType()));
        compGraphJson["valueAfter"] = targetAfter.getValue();
        compGraphJson["unit"] = targetAfter.getUnit().toString();

        compGraphJson["best"] = targetAfter.getTarget().getValueBest();
        compGraphJson["max"] = targetAfter.getTarget().getValueMax();
        compGraphJson["min"] = targetAfter.getTarget().getValueMin();

        compGraphJson["textBefore"] = langMgr.translate("before");
        compGraphJson["textAfter"] = langMgr.translate("after");
        compGraphJson["textRecommendation"] = langMgr.translate("recommendation");
        compGraphJson["textBestTarget"] = langMgr.translate("best_target");
        compGraphJson["textTime"] = langMgr.translate("time");
        compGraphJson["textTherapeuticRange"] = langMgr.translate("therapeutic_range");

        _graphCompDataJson["compgraph_data"].emplace_back(compGraphJson);
    }
}

void XpertRequestResultHtmlExport::getPDAAprioriGraphDataJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _graphDataJson) const
{
    auto percentilesResult = _xpertRequestResult.getPADAprioriPercentiles();

    std::vector<int> tabRank = {5, 10, 25, 50, 75, 90, 95};

    // For each rank, iterate on 24h periods and retrieve time and values.
    for (int rank : tabRank) {
        // for (size_t rank = 1; rank <= 99; ++rank) {
        auto percentileData = percentilesResult->getPercentileData(rank);
        for (const auto& period : percentileData) {
            inja::json periodJson;
            periodJson["rank"] = std::to_string(rank);
            periodJson["start"] = Common::Utils::varToString(period.m_start);

            // Add the time offsets.
            stringstream timesStream;
            for (size_t i = 0; i < period.m_times[0].size(); i++) {
                timesStream << std::to_string(period.m_times[0][i]);
                if (i != period.m_times[0].size() - 1) {
                    timesStream << ',';
                }
            }
            periodJson["times"] = timesStream.str();

            // Add the data values.
            stringstream valuesStream;
            for (size_t i = 0; i < period.m_concentrations[0].size(); i++) {
                valuesStream << std::to_string(period.m_concentrations[0][i]);
                if (i != period.m_concentrations[0].size() - 1) {
                    valuesStream << ',';
                }
            }
            periodJson["values"] = valuesStream.str();

            _graphDataJson["percentiles"].emplace_back(periodJson);
        }
    }
}

void XpertRequestResultHtmlExport::getPDAAposterioriGraphDataJson(
        const XpertRequestResult& _xpertRequestResult, inja::json& _graphDataJson) const
{
    auto concentrations = _xpertRequestResult.getPADAposterioriConcentrations();
    if (concentrations == nullptr) {
        return;
    }

    // For each cycle data
    for (const auto& cycleData : concentrations->getData()) {
        inja::json cycleDataJson;

        // Add the cycle data start time
        cycleDataJson["start"] = Common::Utils::varToString(cycleData.m_start);

        // Add the cycle data offsets
        stringstream timesStream;
        for (size_t i = 0; i < cycleData.m_times[0].size(); i++) {
            timesStream << std::to_string(cycleData.m_times[0][i]);
            if (i != cycleData.m_times[0].size() - 1) {
                timesStream << ',';
            }
        }
        cycleDataJson["times"] = timesStream.str();

        // Add the cycle data values
        stringstream valuesStream;
        for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
            valuesStream << std::to_string(cycleData.m_concentrations[0][i]);
            if (i != cycleData.m_concentrations[0].size() - 1) {
                valuesStream << ',';
            }
        }
        cycleDataJson["values"] = valuesStream.str();
        _graphDataJson["concentrations"].emplace_back(cycleDataJson);
    }
}

std::string XpertRequestResultHtmlExport::prefixDosage(
        const std::string& _dosageIndication, const std::string& _dosageIndicationChain) const
{
    stringstream newDosageIndicationsChainStream;
    newDosageIndicationsChainStream << _dosageIndication // Add a coma only if there is already a dosage indication.
                                    << (_dosageIndicationChain.empty() ? "" : ", ") << _dosageIndicationChain;

    return newDosageIndicationsChainStream.str();
}

} // namespace Xpert
} // namespace Tucuxi
