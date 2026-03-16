// SPDX-License-Identifier: AGPL-3.0-or-later
#include "xpertqueryimport.h"

#include "tucucommon/loggerhelper.h"
#include "tucucommon/xmliterator.h"

#include "tucucdss-core/query/admindata.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

XpertQueryImport::XpertQueryImport() = default;

Common::IImport::Status XpertQueryImport::importFromFile(
        unique_ptr<XpertQueryData>& _query, const std::string& _fileName)
{
    // Ensure that the function is reentrant.
    lock_guard<mutex> lock(m_mutex);

    setStatus(Status::Ok);
    _query = nullptr;

    // Create the xml document from file.
    Tucuxi::Common::XmlDocument document;
    if (!document.open(_fileName)) {
        setStatus(Status::CantCreateXmlDocument, "file could not be opened");
        return Status::CantOpenFile;
    }

    return importDocument(_query, document);
}

Common::IImport::Status XpertQueryImport::importFromString(unique_ptr<XpertQueryData>& _query, const std::string& _xml)
{
    // Ensure that the function is reentrant.
    lock_guard<mutex> lock(m_mutex);

    setStatus(Status::Ok);
    _query = nullptr;

    // Create the xml document from string.
    Tucuxi::Common::XmlDocument document;
    if (!document.fromString(_xml)) {
        setStatus(Status::CantCreateXmlDocument, "xml document could not be created. The tags must be controlled");
        return Status::CantCreateXmlDocument;
    }

    return importDocument(_query, document);
}

Common::IImport::Status XpertQueryImport::importDocument(
        unique_ptr<XpertQueryData>& _query, Common::XmlDocument& _document)
{
    Tucuxi::Common::LoggerHelper logHelper;
    static const std::string QUERY_ID_NODE_NAME = "queryId";
    static const std::string CLIENT_ID_NODE_NAME = "clientId";
    static const std::string DATE_NODE_NAME = "date";
    static const std::string LANGUAGE_NODE_NAME = "language";
    static const std::string REQUESTS_NODE_NAME = "requests";
    static const std::string XPERT_REQUEST_REQUESTS_NODE_NAME = "xpertRequest";

    Common::XmlNode root = _document.getRoot();

    auto queryIdIterator = root.getChildren(QUERY_ID_NODE_NAME);
    std::string queryId;
    if (queryIdIterator->isValid()) {
        queryId = queryIdIterator->getValue();
    }

    auto clientIdIterator = root.getChildren(QUERY_ID_NODE_NAME);
    std::string clientId;
    root.getChildren(CLIENT_ID_NODE_NAME)->getValue();
    if (clientIdIterator->isValid()) {
        clientId = clientIdIterator->getValue();
    }

    Common::XmlNodeIterator languageIterator = root.getChildren(LANGUAGE_NODE_NAME);
    std::string language;
    if (queryIdIterator->isValid()) {
        language = languageIterator->getValue();
    }

    unique_ptr<AdminData> pAdministrativeData = createAdminData(_document);

    unique_ptr<Query::DrugTreatmentData> pParametersData;
    try {
        pParametersData = createDrugTreatmentData(_document);
    }
    catch (std::runtime_error const& e) {
        setStatus(Status::Error, std::string("Error encountered parsing the input TQF file: ") + e.what());
    }

    Common::XmlNodeIterator requestsRootIterator = root.getChildren(REQUESTS_NODE_NAME);
    checkNodeIterator(requestsRootIterator, REQUESTS_NODE_NAME);

    vector<unique_ptr<Query::RequestData> > requests;

    requestsRootIterator = root.getChildren(REQUESTS_NODE_NAME);
    Common::XmlNodeIterator requestsXpertIterator = requestsRootIterator->getChildren(XPERT_REQUEST_REQUESTS_NODE_NAME);
    checkNodeIterator(requestsXpertIterator, XPERT_REQUEST_REQUESTS_NODE_NAME);

    vector<unique_ptr<XpertRequestData> > requestsXpert;
    while (requestsXpertIterator != Common::XmlNodeIterator::none()) {
        requestsXpert.emplace_back(createXpertRequesData(requestsXpertIterator));
        requestsXpertIterator++;
    }

    if (requestsXpert.empty()) {
        setStatus(Status::Error, "No xpertRequest found");
    }

    // We only take the query date here. Indeed, the date might be missing, but
    // it makes sense in that case to replace it with the request date.
    // If at least a (valid) request is present, we do so. In doing so, we check
    // that the missing query date is the only reason the query is rejected ---
    // if this is not the case, we simply quit.
    Common::XmlNodeIterator dateIterator = root.getChildren(DATE_NODE_NAME);
    Common::DateTime date;
    if (!checkNodeIterator(dateIterator, DATE_NODE_NAME).empty()) {
        date = extractDateTime(dateIterator);
    }
    else {
        if (!requestsXpert.empty()) {
            logHelper.warn("Missing query date, replacing it with request date");
            date = requestsXpert.front()->getAdjustmentTime();
            if (getErrorMessage() == "\n<date> not found in xml input\n") {
                setStatus(Status::Ok);
            }
        }
    }

    _query = make_unique<XpertQueryData>(
            queryId,
            clientId,
            date,
            language,
            std::move(pAdministrativeData),
            std::move(pParametersData),
            requests,
            requestsXpert);

    return getStatus();
}

unique_ptr<AdminData> XpertQueryImport::createAdminData(Common::XmlDocument& _document)
{
    Tucuxi::Common::LoggerHelper logHelper;
    static const std::string ADMIN_NODE_NAME = "admin";
    static const std::string MANDATOR_NODE_NAME = "mandator";
    static const std::string PATIENT_NODE_NAME = "patient";
    static const std::string CLINICAL_DATAS_NODE_NAME = "clinicalDatas";
    std::string birthdate;

    Common::XmlNode root = _document.getRoot();

    Common::XmlNodeIterator adminRootIterator = root.getChildren(ADMIN_NODE_NAME);

    // If the node is not present.
    unique_ptr<FullPersonData> pMandator;
    unique_ptr<FullPersonData> pPatient;
    unique_ptr<ClinicalDatas> pClinicalData;

    pMandator = createFullPersonData(root, false);
    pPatient = createFullPersonData(root, true);

    if (!adminRootIterator->isValid()
        || adminRootIterator->getChildren() == Common::XmlIterator<Common::XmlNode>::none()) {
        map<std::string, std::string> data;
        pClinicalData = make_unique<ClinicalDatas>(data);
    }
    else {
        Common::XmlNodeIterator clinicalDataRootIterator = adminRootIterator->getChildren(CLINICAL_DATAS_NODE_NAME);
        pClinicalData = createClinicalDatas(clinicalDataRootIterator);
    }

    return make_unique<AdminData>(std::move(pMandator), std::move(pPatient), std::move(pClinicalData));
}

unique_ptr<FullPersonData> XpertQueryImport::createFullPersonData(Common::XmlNode& _root, bool _isPatient)
{
    static const std::string ADMIN_NODE_NAME = "admin";
    static const std::string MANDATOR_NODE_NAME = "mandator";
    static const std::string PATIENT_NODE_NAME = "patient";
    static const std::string PERSON_NODE_NAME = "person";
    static const std::string INSTITUTE_NODE_NAME = "institute";

    Common::XmlNodeIterator adminRootIterator = _root.getChildren(ADMIN_NODE_NAME);

    Common::XmlNodeIterator mandatorPatientRootIterator = _isPatient
                                                                  ? adminRootIterator->getChildren(PATIENT_NODE_NAME)
                                                                  : adminRootIterator->getChildren(MANDATOR_NODE_NAME);

    unique_ptr<PersonData> pPersonalContact;
    unique_ptr<InstituteData> pInstituteContact;
    if (!mandatorPatientRootIterator->isValid()) {
        pPersonalContact = createEmptyPersonData();
        pInstituteContact = createEmptyInstituteData();
    }
    else {
        Common::XmlNodeIterator iterator = mandatorPatientRootIterator->getChildren(PERSON_NODE_NAME);
        std::optional<std::string> birthdate = _isPatient ? extractBirthdate(_root) : std::nullopt;
        pPersonalContact = createPersonData(iterator, birthdate);

        iterator = mandatorPatientRootIterator->getChildren(INSTITUTE_NODE_NAME);
        pInstituteContact = createInstituteData(iterator);
    }
    return make_unique<FullPersonData>(std::move(pPersonalContact), std::move(pInstituteContact));
}

unique_ptr<PersonData> XpertQueryImport::createPersonData(
        Common::XmlNodeIterator& _personDataRootIterator, const std::optional<std::string>& _birthdate = std::nullopt)
{
    Tucuxi::Common::LoggerHelper logHelper;
    if (!_personDataRootIterator->isValid()) {
        logHelper.warn("Invalid personal data, setting empty fields");
        return createEmptyPersonData();
    }

    static const std::string ID_NODE_NAME = "id";
    static const std::string TITLE_NODE_NAME = "title";
    static const std::string FIRSTNAME_NODE_NAME = "firstName";
    static const std::string LASTNAME_NODE_NAME = "lastName";
    static const std::string ADDRESS_NODE_NAME = "address";
    static const std::string PHONE_NODE_NAME = "phone";
    static const std::string EMAIL_NODE_NAME = "email";

    std::string id = getChildStringOptional(_personDataRootIterator, ID_NODE_NAME, "");
    std::string title = getChildStringOptional(_personDataRootIterator, TITLE_NODE_NAME, "");
    std::string firstname = getChildString(_personDataRootIterator, FIRSTNAME_NODE_NAME);
    std::string lastname = getChildString(_personDataRootIterator, LASTNAME_NODE_NAME);
    Common::XmlNodeIterator iterator = _personDataRootIterator->getChildren(ADDRESS_NODE_NAME);
    unique_ptr<AddressData> pAddress = createAddressData(iterator);
    iterator = _personDataRootIterator->getChildren(PHONE_NODE_NAME);
    unique_ptr<PhoneData> pPhone = createPhoneData(iterator);
    iterator = _personDataRootIterator->getChildren(EMAIL_NODE_NAME);
    unique_ptr<EmailData> pEmail = createEmailData(iterator);

    return make_unique<PersonData>(
            id, title, firstname, lastname, std::move(pAddress), std::move(pPhone), std::move(pEmail), _birthdate);
}

unique_ptr<PersonData> XpertQueryImport::createEmptyPersonData()
{
    unique_ptr<AddressData> pAddress = createEmptyAddressData();
    unique_ptr<PhoneData> pPhone = createEmptyPhoneData();
    unique_ptr<EmailData> pEmail = createEmptyEmailData();

    Tucuxi::Common::LoggerHelper logHelper;
    logHelper.warn("Setting empty person data!");

    return make_unique<PersonData>(
            "", "", "", "", std::move(pAddress), std::move(pPhone), std::move(pEmail), std::nullopt);
}


std::optional<std::string> XpertQueryImport::extractBirthdate(Common::XmlNode& _root)
{
    Tucuxi::Common::LoggerHelper logHelper;
    static const std::string DRUGTREATMENT_NODE_NAME = "drugTreatment";
    static const std::string PATIENT_NODE_NAME = "patient";
    static const std::string COVARIATES_NODE_NAME = "covariates";
    static const std::string COVARIATEID_NODE_NAME = "covariateId";
    static const std::string VALUE_NODE_NAME = "value";

    Common::XmlNodeIterator parametersRootIterator = _root.getChildren(DRUGTREATMENT_NODE_NAME);
    if (!parametersRootIterator->isValid()) {
        return std::nullopt;
    }

    Common::XmlNodeIterator patientCovariates = parametersRootIterator->getChildren(PATIENT_NODE_NAME);
    if (!patientCovariates->isValid()) {
        return std::nullopt;
    }

    Common::XmlNodeIterator covariatesRootIterator = patientCovariates->getChildren(COVARIATES_NODE_NAME);
    if (!covariatesRootIterator->isValid()) {
        return std::nullopt;
    }

    Common::XmlNodeIterator covariatesIterator = covariatesRootIterator->getChildren();
    while (covariatesIterator != Common::XmlNodeIterator::none()) {
        std::string covariateId = getChildString(covariatesIterator, COVARIATEID_NODE_NAME);
        if (covariateId == "birthdate") {
            Common::DateTime value = getChildDateTime(covariatesIterator, VALUE_NODE_NAME);
            return Tucuxi::Common::Utils::varToString(value);
        }
        covariatesIterator++;
    }

    return std::nullopt;
}

unique_ptr<InstituteData> XpertQueryImport::createInstituteData(Common::XmlNodeIterator& _instituteRootIterator)
{
    Tucuxi::Common::LoggerHelper logHelper;
    if (!_instituteRootIterator->isValid()) {
        logHelper.warn("Invalid institute data, setting empty fields");
        return createEmptyInstituteData();
    }

    static const std::string ID_NODE_NAME = "id";
    static const std::string NAME_NODE_NAME = "name";
    static const std::string ADDRESS_NODE_NAME = "address";
    static const std::string PHONE_NODE_NAME = "phone";
    static const std::string EMAIL_NODE_NAME = "email";

    std::string id = getChildStringOptional(_instituteRootIterator, ID_NODE_NAME, "");
    std::string name = getChildString(_instituteRootIterator, NAME_NODE_NAME);
    Common::XmlNodeIterator iterator = _instituteRootIterator->getChildren(ADDRESS_NODE_NAME);
    unique_ptr<AddressData> pAddress = createAddressData(iterator);
    iterator = _instituteRootIterator->getChildren(PHONE_NODE_NAME);
    unique_ptr<PhoneData> pPhone = createPhoneData(iterator);
    iterator = _instituteRootIterator->getChildren(EMAIL_NODE_NAME);
    unique_ptr<EmailData> pEmail = createEmailData(iterator);

    return make_unique<InstituteData>(id, name, std::move(pAddress), std::move(pPhone), std::move(pEmail));
}

unique_ptr<InstituteData> XpertQueryImport::createEmptyInstituteData()
{
    unique_ptr<AddressData> pAddress = createEmptyAddressData();
    unique_ptr<PhoneData> pPhone = createEmptyPhoneData();
    unique_ptr<EmailData> pEmail = createEmptyEmailData();

    Tucuxi::Common::LoggerHelper logHelper;
    logHelper.warn("Setting empty institute data!");

    return make_unique<InstituteData>("", "", std::move(pAddress), std::move(pPhone), std::move(pEmail));
}


unique_ptr<AddressData> XpertQueryImport::createAddressData(Common::XmlNodeIterator& _addressRootIterator)
{
    Tucuxi::Common::LoggerHelper logHelper;
    if (!_addressRootIterator->isValid()) {
        logHelper.warn("Invalid address, setting empty fields");
        return createEmptyAddressData();
    }

    static const std::string STREET_NODE_NAME = "street";
    static const std::string POSTALCODE_NODE_NAME = "postalCode";
    static const std::string CITY_NODE_NAME = "city";
    static const std::string STATE_NODE_NAME = "state";
    static const std::string COUNTRY_NODE_NAME = "country";

    std::string street = getChildString(_addressRootIterator, STREET_NODE_NAME);
    std::string postalCode = getChildString(_addressRootIterator, POSTALCODE_NODE_NAME);
    std::string city = getChildString(_addressRootIterator, CITY_NODE_NAME);
    std::string state = getChildStringOptional(_addressRootIterator, STATE_NODE_NAME, "");
    std::string country = getChildStringOptional(_addressRootIterator, COUNTRY_NODE_NAME, "");

    return make_unique<AddressData>(street, postalCode, city, state, country);
}

unique_ptr<AddressData> XpertQueryImport::createEmptyAddressData()
{
    return make_unique<AddressData>("", "", "", "", "");
}


unique_ptr<PhoneData> XpertQueryImport::createPhoneData(Common::XmlNodeIterator& _phoneRootIterator)
{
    Tucuxi::Common::LoggerHelper logHelper;
    if (!_phoneRootIterator->isValid()) {
        logHelper.warn("Invalid phone data, setting empty fields");
        return createEmptyPhoneData();
    }

    static const std::string NUMBER_NODE_NAME = "number";
    static const std::string TYPE_NODE_NAME = "type";

    std::string number = getChildString(_phoneRootIterator, NUMBER_NODE_NAME);
    std::string type = getChildStringOptional(_phoneRootIterator, TYPE_NODE_NAME, "");

    return make_unique<PhoneData>(number, type);
}

unique_ptr<PhoneData> XpertQueryImport::createEmptyPhoneData()
{
    return make_unique<PhoneData>("", "");
}


unique_ptr<EmailData> XpertQueryImport::createEmailData(Common::XmlNodeIterator& _emailRootIterator)
{
    Tucuxi::Common::LoggerHelper logHelper;
    if (!_emailRootIterator->isValid()) {
        logHelper.warn("Invalid e-mail data, setting empty fields");
        return createEmptyEmailData();
    }

    static const std::string ADDRESS_NODE_NAME = "address";
    static const std::string TYPE_NODE_NAME = "type";

    std::string address = getChildString(_emailRootIterator, ADDRESS_NODE_NAME);
    std::string type = getChildStringOptional(_emailRootIterator, TYPE_NODE_NAME, "");

    return make_unique<EmailData>(address, type);
}

unique_ptr<EmailData> XpertQueryImport::createEmptyEmailData()
{
    return make_unique<EmailData>("", "");
}


unique_ptr<ClinicalDatas> XpertQueryImport::createClinicalDatas(Common::XmlNodeIterator& _clinicalDatasRootIterator)
{
    if (!_clinicalDatasRootIterator->isValid()) {
        return nullptr;
    }

    static const std::string CLINICAL_DATA_NODE_NAME = "clinicalData";

    map<std::string, std::string> data;

    Common::XmlNodeIterator xmlNodeIterator = _clinicalDatasRootIterator->getChildren(CLINICAL_DATA_NODE_NAME);

    // For each clinical data
    while (xmlNodeIterator != Common::XmlNodeIterator::none()) {
        data[xmlNodeIterator->getAttribute("key").getValue()] = xmlNodeIterator->getValue();
        xmlNodeIterator++;
    }

    return make_unique<ClinicalDatas>(data);
}

unique_ptr<XpertRequestData> XpertQueryImport::createXpertRequesData(Common::XmlNodeIterator& _xpertRequestRootIterator)
{
    static const std::string DRUG_ID_NODE_NAME = "drugId";
    static const std::string CONFIG_ID_NODE_NAME = "configId";
    static const std::string OUTPUT_NODE_NAME = "output";
    static const std::string DATE_ADJUSTMENT_TIME_NODE_NAME = "adjustmentDate";
    static const std::string OPTIONS_NODE_NAME = "options";

    std::string drugId = getChildString(_xpertRequestRootIterator, DRUG_ID_NODE_NAME);
    std::string configId = getChildString(_xpertRequestRootIterator, CONFIG_ID_NODE_NAME);

    Common::XmlNodeIterator outputRootIterator = _xpertRequestRootIterator->getChildren(OUTPUT_NODE_NAME);

    static const std::string FORMAT_NODE_NAME = "format";
    static const std::string LANGUAGE_NODE_NAME = "language";

    std::string formatStr = getChildString(outputRootIterator, FORMAT_NODE_NAME);
    OutputFormat format;

    if (formatStr == "xml") {
        format = OutputFormat::XML;
    }
    else if (formatStr == "html") {
        format = OutputFormat::HTML;
    }
    else if (formatStr == "pdf") {
        format = OutputFormat::PDF;
    }
    else {
        setStatus(Status::Error, "Unknown output format");
    }

    std::string languageStr = getChildString(outputRootIterator, LANGUAGE_NODE_NAME);

    OutputLang language;

    if (languageStr == "en") {
        language = OutputLang::ENGLISH;
    }
    else if (languageStr == "fr") {
        language = OutputLang::FRENCH;
    }
    else {
        setStatus(Status::Error, "Unknown output language");
    }

    Common::DateTime adjustmentTime = getChildDateTime(_xpertRequestRootIterator, DATE_ADJUSTMENT_TIME_NODE_NAME);

    LoadingOption loadingOption =
            getChildLoadingOptionEnumOptional(_xpertRequestRootIterator, OPTIONS_NODE_NAME, LoadingOption::Unspecified);

    RestPeriodOption restPeriodOption = getChildRestPeriodOptionEnumOptional(
            _xpertRequestRootIterator, OPTIONS_NODE_NAME, RestPeriodOption::Unspecified);

    Tucuxi::Core::TargetExtractionOption targetExtractionOption = getChildTargetExtractionOptionEnumOptional(
            _xpertRequestRootIterator, OPTIONS_NODE_NAME, Core::TargetExtractionOption::DefinitionIfNoIndividualTarget);

    Tucuxi::Core::FormulationAndRouteSelectionOption formulationAndRouteSelectionOption =
            getChildFormulationAndRouteSelectionOptionEnumOptional(
                    _xpertRequestRootIterator,
                    OPTIONS_NODE_NAME,
                    Core::FormulationAndRouteSelectionOption::LastFormulationAndRoute);


    return make_unique<XpertRequestData>(
            drugId,
            format,
            language,
            adjustmentTime,
            loadingOption,
            restPeriodOption,
            targetExtractionOption,
            formulationAndRouteSelectionOption,
            configId);
}

std::string XpertQueryImport::getChildStringOptional(
        Common::XmlNodeIterator _rootIterator, const std::string& _childName, const std::string& _default)
{
    auto child = _rootIterator->getChildren(_childName);

    if (child == Common::XmlNodeIterator::none()) {
        return _default;
    }
    return extractString(child);
}

LoadingOption XpertQueryImport::getChildLoadingOptionEnumOptional(
        Common::XmlNodeIterator _xpertRequestRootIterator, const std::string& _childName, LoadingOption _default)
{
    static const std::string LOADING_OPTION_NODE = "loadingOption";

    // Get the node conatining the loading option node.
    Common::XmlNodeIterator optionsRootIterator = _xpertRequestRootIterator->getChildren(_childName);

    Common::XmlNodeIterator loadingOptionRootIterator = optionsRootIterator->getChildren(LOADING_OPTION_NODE);


    static map<std::string, LoadingOption> m = {
            {"noLoadingDose", LoadingOption::NoLoadingDose}, {"loadingDoseAllowed", LoadingOption::LoadingDoseAllowed}};

    std::string value = loadingOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    return _default;
}

RestPeriodOption XpertQueryImport::getChildRestPeriodOptionEnumOptional(
        Common::XmlNodeIterator _xpertRequestRootIterator, const std::string& _childName, RestPeriodOption _default)
{

    static const std::string REST_PERIOD_OPTION_NODE = "restPeriodOption";

    // Get the node conatining the rest period option node.
    Common::XmlNodeIterator optionsRootIterator = _xpertRequestRootIterator->getChildren(_childName);

    Common::XmlNodeIterator restPeriodOptionRootIterator = optionsRootIterator->getChildren(REST_PERIOD_OPTION_NODE);


    static map<std::string, RestPeriodOption> m = {
            {"noRestPeriod", RestPeriodOption::NoRestPeriod},
            {"restPeriodAllowed", RestPeriodOption::RestPeriodAllowed}};

    std::string value = restPeriodOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    return _default;
}

Core::TargetExtractionOption XpertQueryImport::getChildTargetExtractionOptionEnumOptional(
        Common::XmlNodeIterator _xpertRequestRootIterator,
        const std::string& _childName,
        Core::TargetExtractionOption _default)
{

    static const std::string TARGET_EXTRACTION_OPTION_NODE = "targetExtractionOption";

    // Get the node conatining the target extraction option node.
    Common::XmlNodeIterator optionsRootIterator = _xpertRequestRootIterator->getChildren(_childName);

    Common::XmlNodeIterator targetExtractionOptionRootIterator =
            optionsRootIterator->getChildren(TARGET_EXTRACTION_OPTION_NODE);


    static map<std::string, Tucuxi::Core::TargetExtractionOption> m = {
            {"populationValues", Tucuxi::Core::TargetExtractionOption::PopulationValues},
            {"aprioriValues", Tucuxi::Core::TargetExtractionOption::AprioriValues},
            {"individualTargets", Tucuxi::Core::TargetExtractionOption::IndividualTargets},
            {"individualTargetsIfDefinitionExists",
             Tucuxi::Core::TargetExtractionOption::IndividualTargetsIfDefinitionExists},
            {"definitionIfNoIndividualTarget", Tucuxi::Core::TargetExtractionOption::DefinitionIfNoIndividualTarget},
            {"individualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget",
             Tucuxi::Core::TargetExtractionOption::IndividualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget}

    };

    std::string value = targetExtractionOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    return _default;
}

Core::FormulationAndRouteSelectionOption XpertQueryImport::getChildFormulationAndRouteSelectionOptionEnumOptional(
        Common::XmlNodeIterator _xpertRequestRootIterator,
        const std::string& _childName,
        Core::FormulationAndRouteSelectionOption _default)
{

    static const std::string FORMULATION_AND_ROUTE_SELECTION_OPTION_NODE = "formulationAndRouteSelectionOption";

    // Get the node conatining the formulation and route selection option node.
    Common::XmlNodeIterator optionsRootIterator = _xpertRequestRootIterator->getChildren(_childName);

    Common::XmlNodeIterator formulationAndRouteSelectionOptioneRootIterator =
            optionsRootIterator->getChildren(FORMULATION_AND_ROUTE_SELECTION_OPTION_NODE);


    static map<std::string, Tucuxi::Core::FormulationAndRouteSelectionOption> m = {
            {"allFormulationAndRoutes", Tucuxi::Core::FormulationAndRouteSelectionOption::AllFormulationAndRoutes},
            {"lastFormulationAndRoute", Tucuxi::Core::FormulationAndRouteSelectionOption::LastFormulationAndRoute},
            {"defaultFormulationAndRoute",
             Tucuxi::Core::FormulationAndRouteSelectionOption::DefaultFormulationAndRoute}};

    std::string value = formulationAndRouteSelectionOptioneRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    return _default;
}

Common::DateTime XpertQueryImport::getChildDateTime(
        Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    Common::XmlNodeIterator child = _rootIterator->getChildren(_childName);

    if (!child->isValid()) {
        return DateTime::undefinedDateTime();
    }
    return extractDateTime(child);
}

} // namespace Xpert
} // namespace Tucuxi
