// SPDX-License-Identifier: AGPL-3.0-or-later
#include <fstream>
#include <string>

#include "xpertrequestresultxmlexport.h"

#include "tucucommon/utils.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"

#include "tucuquery/fullsample.h"

#include "tucucdss-core/result/xpertqueryresult.h"
#include "tucucdss-core/utils/xpertutils.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

void XpertRequestResultXmlExport::exportToFile(XpertRequestResult& _xpertRequestResult)
{
    // Save the reference on the xpertRequestResult to be able to retrieve
    // it anywhere and anytime. For example, when exporting
    // single doses and to be able to retrieve the associated validation results.
    m_xpertRequestResultInUse = &_xpertRequestResult;

    // Get the xml string.
    std::string xmlString;
    makeXmlString(_xpertRequestResult, xmlString);

    // Get the filename <drugId>_<requestNumber>_<current time>.<extension>
    std::string fileName = computeFileName(_xpertRequestResult);

    // Opening the file.
    ofstream file;
    file.open(fileName);
    if ((file.rdstate() & ostream::failbit) != 0) {
        _xpertRequestResult.setErrorMessage("The file " + fileName + " could not be opened.");
        return;
    }

    // Write & close.
    file << xmlString;
    file.close();

    // The xml is exported.
}

void XpertRequestResultXmlExport::makeXmlString(const XpertRequestResult& _xpertRequestResult, std::string& _xmlString)
{
    // Making root.
    Tucuxi::Common::XmlNode root = m_xmlDocument.createNode(Tucuxi::Common::EXmlNodeType::Element, "tuberxpertResult");
    auto attribute1 = m_xmlDocument.createAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.addAttribute(attribute1);
    auto attribute2 =
            m_xmlDocument.createAttribute("xsi:noNamespaceSchemaLocation", "tuberxpert_computing_response.xsd");
    root.addAttribute(attribute2);

    m_xmlDocument.setRoot(root);

    // Computation time.
    addNode(root,
            "computationTime",
            dateTimeToXmlString(_xpertRequestResult.getXpertQueryResult().getComputationTime()));

    // Language.
    addNode(root, "language", outputLangToString(_xpertRequestResult.getXpertRequest().getOutputLang()));

    // Add the intro drugId, modelId, lastDose.
    exportDrugIntro(_xpertRequestResult, root);

    // Add the admin.
    exportAdminData(_xpertRequestResult.getXpertQueryResult().getAdminData(), root);

    // Add the covariates.
    exportCovariateResults(_xpertRequestResult.getCovariateValidationResults(), root);

    // Add the dosage history (treatment).
    exportTreatment(_xpertRequestResult.getTreatment(), root);

    // Add the samples.
    exportSampleResults(_xpertRequestResult.getSampleValidationResults(), root);

    // Add the adjustments.
    exportAdjustmentData(_xpertRequestResult.getAdjustmentData(), root);

    // Add the parameters.
    exportParameters(_xpertRequestResult, root);

    // Add statistics.
    exportStatistics(_xpertRequestResult, root);

    // Add computationCovariates.
    exportComputationCovariates(_xpertRequestResult, root);

    m_xmlDocument.toString(_xmlString, true);
}

void XpertRequestResultXmlExport::exportDrugIntro(
        const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode)
{
    // <drug>
    Common::XmlNode drugNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "drug");
    _rootNode.addChild(drugNode);

    //  <drugId>
    addNode(drugNode, "drugId", _xpertRequestResult.getXpertRequest().getDrugId());

    //      <lastDose>
    Common::XmlNode lastDoseNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "lastDose");
    drugNode.addChild(lastDoseNode);

    // If there is a last dose.
    if (_xpertRequestResult.getLastIntake() != nullptr) {
        //              <value>
        addNode(lastDoseNode, "value", doubleToString(_xpertRequestResult.getLastIntake()->getDose()));

        //              <unit>
        addNode(lastDoseNode, "unit", _xpertRequestResult.getLastIntake()->getUnit().toString());
    }

    //  <drugModelId>
    addNode(drugNode, "drugModelId", _xpertRequestResult.getDrugModel()->getDrugModelId());
}

void XpertRequestResultXmlExport::exportAdminData(const AdminData* _admin, Common::XmlNode& _rootNode)
{
    // We export the admin if it contains at least one of its elements.
    if (_admin == nullptr
        || (_admin->getMandator() == nullptr && _admin->getPatient() == nullptr
            && _admin->getClinicalDatas() == nullptr)) {
        return;
    }

    // <admin>
    Common::XmlNode adminNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "admin");
    _rootNode.addChild(adminNode);

    //   <mandator>
    exportFullPersonData(_admin->getMandator(), adminNode, "mandator");

    //   <patient>
    exportFullPersonData(_admin->getPatient(), adminNode, "patient");

    //   <clinicalDatas>
    exportClinicalDatas(_admin->getClinicalDatas(), adminNode);
}

void XpertRequestResultXmlExport::exportFullPersonData(
        const FullPersonData* _fullPerson, Common::XmlNode& _adminNode, const std::string& _nodeName)
{
    // If the person to export is not present, just leave.
    if (_fullPerson == nullptr) {
        return;
    }

    // <mandator> or <patient>
    Common::XmlNode patientMandatorNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, _nodeName);
    _adminNode.addChild(patientMandatorNode);

    //   <person>
    exportPersonData(_fullPerson->getPerson(), patientMandatorNode);

    //   <institute>
    exportInstituteData(_fullPerson->getInstitute(), patientMandatorNode);
}

void XpertRequestResultXmlExport::exportPersonData(const PersonData& _person, Common::XmlNode& _patientMandatorNode)
{
    // <person>
    Common::XmlNode personNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "person");
    _patientMandatorNode.addChild(personNode);

    //   <id>
    if (!_person.getId().empty()) {
        addNode(personNode, "id", _person.getId());
    }

    //   <title>
    if (!_person.getTitle().empty()) {
        addNode(personNode, "title", _person.getTitle());
    }

    //   <firstName>
    addNode(personNode, "firstName", _person.getFirstName());

    //   <lastName>
    addNode(personNode, "lastName", _person.getLastName());

    //   <address>
    exportAddressData(_person.getAddress(), personNode);

    //   <phone>
    exportPhoneData(_person.getPhone(), personNode);

    //   <email>
    exportEmailData(_person.getEmail(), personNode);
}

void XpertRequestResultXmlExport::exportInstituteData(
        const InstituteData* _institute, Common::XmlNode& _patientMandatorNode)
{
    // If no institute, just leave
    if (_institute == nullptr) {
        return;
    }

    // <institute>
    Common::XmlNode instituteNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "institute");
    _patientMandatorNode.addChild(instituteNode);


    //   <id>
    if (!_institute->getId().empty()) {
        addNode(instituteNode, "id", _institute->getId());
    }

    //   <name>
    addNode(instituteNode, "name", _institute->getName());

    //   <address>
    exportAddressData(_institute->getAddress(), instituteNode);

    //   <phone>
    exportPhoneData(_institute->getPhone(), instituteNode);

    //   <email>
    exportEmailData(_institute->getEmail(), instituteNode);
}

void XpertRequestResultXmlExport::exportAddressData(const AddressData* _address, Common::XmlNode& _personInstituteNode)
{
    if (_address == nullptr) {
        return;
    }

    // <address>
    Common::XmlNode addressNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "address");
    _personInstituteNode.addChild(addressNode);

    //   <street>
    addNode(addressNode, "street", _address->getStreet());

    //   <postalCode>
    addNode(addressNode, "postalCode", _address->getPostalCode());

    //   <city>
    addNode(addressNode, "city", _address->getCity());

    //   <state>
    if (!_address->getState().empty()) {
        addNode(addressNode, "state", _address->getState());
    }

    //   <country>
    if (!_address->getCountry().empty()) {
        addNode(addressNode, "country", _address->getCountry());
    }
}

void XpertRequestResultXmlExport::exportPhoneData(const PhoneData* _phone, Common::XmlNode& _personInstituteNode)
{
    if (_phone == nullptr) {
        return;
    }

    // <phone>
    Common::XmlNode phoneNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "phone");
    _personInstituteNode.addChild(phoneNode);

    //   <number>
    addNode(phoneNode, "number", _phone->getNumber());

    //   <type>
    if (!_phone->getType().empty()) {
        addNode(phoneNode, "type", _phone->getType());
    }
}

void XpertRequestResultXmlExport::exportEmailData(const EmailData* _email, Common::XmlNode& _personInstituteNode)
{
    if (_email == nullptr) {
        return;
    }

    // <phone>
    Common::XmlNode emailNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "email");
    _personInstituteNode.addChild(emailNode);

    //   <number>
    addNode(emailNode, "address", _email->getAddress());

    //   <type>
    if (!_email->getType().empty()) {
        addNode(emailNode, "type", _email->getType());
    }
}

void XpertRequestResultXmlExport::exportClinicalDatas(const ClinicalDatas* _clinicalDatas, Common::XmlNode& _adminNode)
{
    if (_clinicalDatas == nullptr) {
        return;
    }

    // <clinicalDatas>
    Common::XmlNode clinicalDataNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "clinicalDatas");
    _adminNode.addChild(clinicalDataNode);

    for (const auto& entryIt : _clinicalDatas->getData()) {
        //   <clinicalData key="...">
        Common::XmlNode clinicalDataEntryNode =
                m_xmlDocument.createNode(Common::EXmlNodeType::Element, "clinicalData", entryIt.second);
        auto keyAttribute = m_xmlDocument.createAttribute("key", entryIt.first);
        clinicalDataEntryNode.addAttribute(keyAttribute);
        clinicalDataNode.addChild(clinicalDataEntryNode);
    }
}

void XpertRequestResultXmlExport::exportCovariateResults(
        const vector<CovariateValidationResult>& _covariateResults, Common::XmlNode& _rootNode)
{
    // <covariates>
    Common::XmlNode covariatesNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "covariates");
    _rootNode.addChild(covariatesNode);

    // For each covariate validation result
    for (const CovariateValidationResult& covariateValidationResult : _covariateResults) {
        //   <covariate>
        Common::XmlNode covariateNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "covariate");
        covariatesNode.addChild(covariateNode);

        //       <covariateId>
        addNode(covariateNode, "covariateId", covariateValidationResult.getSource()->getId());

        //       <date>
        if (covariateValidationResult.getPatientCovariate() != nullptr) {
            addNode(covariateNode,
                    "date",
                    dateTimeToXmlString(covariateValidationResult.getPatientCovariate()->getEventTime()));
        }

        //       <name>
        addNode(covariateNode,
                "name",
                getStringWithEnglishFallback(
                        covariateValidationResult.getSource()->getName().getString(),
                        m_xpertRequestResultInUse->getXpertRequest().getOutputLang()));

        //       <value>
        if (covariateValidationResult.getSource()->getId() == "age"
            && covariateValidationResult.getPatientCovariate() != nullptr) {
            int age = static_cast<int>(getAgeIn(
                    covariateValidationResult.getSource()->getType(),
                    covariateValidationResult.getPatientCovariate()->getValueAsDate(),
                    m_xpertRequestResultInUse->getXpertQueryResult().getComputationTime()));
            addNode(covariateNode, "value", std::to_string(age));
        }
        else {
            addNode(covariateNode, "value", covariateValidationResult.getValue());
        }

        //       <unit>
        addNode(covariateNode, "unit", covariateValidationResult.getUnit().toString());

        //       <datatype>
        std::string dataType = dataTypeToString(covariateValidationResult.getDataType());

        // If the source is "age" the data type is int.
        if (covariateValidationResult.getSource()->getId() == "age") {
            dataType = dataTypeToString(Core::DataType::Double);
        }

        addNode(covariateNode, "dataType", dataType);

        //       <desc>
        addNode(covariateNode,
                "desc",
                getStringWithEnglishFallback(
                        covariateValidationResult.getSource()->getDescription(),
                        m_xpertRequestResultInUse->getXpertRequest().getOutputLang()));

        //       <source>
        addNode(covariateNode, "source", covariateTypeToString(covariateValidationResult.getType()));

        //       <warning>
        exportWarning(covariateValidationResult, covariateNode);
    }
}

void XpertRequestResultXmlExport::exportTreatment(const Core::DrugTreatment* _treatment, Common::XmlNode& _rootNode)
{
    // <treatment>
    Common::XmlNode treatmentNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "treatment");
    _rootNode.addChild(treatmentNode);

    if (_treatment == nullptr) {
        return;
    }

    //   <dosageHistory>
    exportDosageHistory(_treatment->getDosageHistory(), treatmentNode);
}

void XpertRequestResultXmlExport::exportDose(
        const Tucuxi::Core::SingleDose& _dosage, Tucuxi::Common::XmlNode& _parentNode)
{
    Tucuxi::Common::XmlNode doseNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dose");
    _parentNode.addChild(doseNode);

    // <warning>
    addNode(doseNode, "value", static_cast<double>(_dosage.getDose()));

    // <unit>
    addNode(doseNode, "unit", _dosage.getDoseUnit().toString());

    // <infusionTimeInMinutes>
    addNode(doseNode, "infusionTimeInMinutes", _dosage.getInfusionTime().toMinutes());

    // <warning>
    auto singleDoseIt = m_xpertRequestResultInUse->getDoseValidationResults().find(&_dosage);
    if (singleDoseIt != m_xpertRequestResultInUse->getDoseValidationResults().end()) {
        exportWarning(singleDoseIt->second, doseNode);
    }
}

void XpertRequestResultXmlExport::exportSampleResults(
        const vector<SampleValidationResult>& _sampleResults, Common::XmlNode& _rootNode)
{
    // <samples>
    Common::XmlNode samplesNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "samples");
    _rootNode.addChild(samplesNode);

    // For each sample validation result
    for (const auto& sampleValidationResult : _sampleResults) {
        //   <sample>
        Common::XmlNode sampleNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "sample");
        samplesNode.addChild(sampleNode);

        //       <sampleId>
        const Query::FullSample* fullSample =
                dynamic_cast<const Query::FullSample*>(sampleValidationResult.getSource());
        if (fullSample != nullptr) {
            addNode(sampleNode, "sampleId", fullSample->getSampleId());
        }

        //       <sampleDate>
        addNode(sampleNode, "sampleDate", dateTimeToXmlString(sampleValidationResult.getSource()->getDate()));

        //       <concentrations>
        Common::XmlNode concentrationsNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "concentrations");
        sampleNode.addChild(concentrationsNode);

        // One day when the samples will contain multiple concentrations, use a for loop on them.

        //          <concentration>
        Common::XmlNode concentrationNode = m_xmlDocument.createNode(Common::EXmlNodeType::Element, "concentration");
        concentrationsNode.addChild(concentrationNode);

        //              <analyteId>
        addNode(concentrationNode, "analyteId", sampleValidationResult.getSource()->getAnalyteID().toString());

        //              <percentile>
        addNode(concentrationNode, "percentile", sampleValidationResult.getGroupNumberOver99Percentile());

        //              <value>
        addNode(concentrationNode, "value", sampleValidationResult.getSource()->getValue());

        //              <unit>
        addNode(concentrationNode, "unit", sampleValidationResult.getSource()->getUnit().toString());

        //              <warning>
        exportWarning(sampleValidationResult, concentrationNode);
    }
}

void XpertRequestResultXmlExport::exportAdjustmentData(
        const Core::AdjustmentData* _adjustmentData, Common::XmlNode& _rootNode)
{
    // <dataAdjustment>
    Tucuxi::Common::XmlNode dataAdjustmentNode =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dataAdjustment");
    _rootNode.addChild(dataAdjustmentNode);

    //   <analyteIds>
    Tucuxi::Common::XmlNode analyteIdsNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "analyteIds");
    dataAdjustmentNode.addChild(analyteIdsNode);
    for (const auto& comp : _adjustmentData->getCompartmentInfos()) {

        //      <analyteId>
        addNode(analyteIdsNode, "analyteId", comp.getId());
    }

    //   <adjustments>
    Tucuxi::Common::XmlNode adjustmentsNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "adjustments");
    dataAdjustmentNode.addChild(adjustmentsNode);

    // For each adjustment
    for (const auto& adj : _adjustmentData->getAdjustments()) {

        //      <adjustment>
        Tucuxi::Common::XmlNode adjustmentNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "adjustment");
        adjustmentsNode.addChild(adjustmentNode);

        //          <score>
        addNode(adjustmentNode, "score", adj.getGlobalScore());

        //          <targetEvaluations>
        Tucuxi::Common::XmlNode targetEvaluationsNode =
                m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "targetEvaluations");
        adjustmentNode.addChild(targetEvaluationsNode);

        // For each target evaluation
        for (const auto& target : adj.m_targetsEvaluation) {

            //              <targetEvaluation>
            Tucuxi::Common::XmlNode targetEvaluationNode =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "targetEvaluation");
            targetEvaluationsNode.addChild(targetEvaluationNode);

            //                  <targetType>
            addNode(targetEvaluationNode, "targetType", toString(target.getTargetType()));

            //                  <unit>
            addNode(targetEvaluationNode, "unit", target.getUnit().toString());

            //                  <value>
            addNode(targetEvaluationNode, "value", target.getValue());

            //                  <score>
            addNode(targetEvaluationNode, "score", target.getScore());

            //                  <min>
            addNode(targetEvaluationNode, "min", target.getTarget().getValueMin());

            //                  <best>
            addNode(targetEvaluationNode, "best", target.getTarget().getValueBest());

            //                  <max>
            addNode(targetEvaluationNode, "max", target.getTarget().getValueMax());


            // --------------- /!\ TO ADD WHEN ADJUSTMENT DATA HAS TARGET INEFFICACY AND TOXICITY /!\ -----------

            //            //                  <inefficacyAlarm>
            //            addNode(targetEvaluationNode, "inefficacyAlarm", target.getTarget().getInefficacyAlarm());

            //            //                  <toxicityAlarm>
            //            addNode(targetEvaluationNode, "toxicityAlarm", target.getTarget().getToxicityAlarm());

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
            double inefficacyAlarm = targetDefinitionIt != modelTargets.end()
                                             ? (*targetDefinitionIt)->getInefficacyAlarm().getValue()
                                             : -1;
            double toxicityAlarm = targetDefinitionIt != modelTargets.end()
                                           ? (*targetDefinitionIt)->getToxicityAlarm().getValue()
                                           : -1;

            //                  <inefficacyAlarm>
            addNode(targetEvaluationNode, "inefficacyAlarm", inefficacyAlarm);

            //                  <toxicityAlarm>
            addNode(targetEvaluationNode, "toxicityAlarm", toxicityAlarm);

            // --------------- /!\                      END TO REMOVE                                 /!\ -----------
        }

        //          <dosageHistory>
        exportDosageHistory(adj.m_history, adjustmentNode);

        //          <cycleDatas>
        exportCycleDatas(adj.getData(), adjustmentNode);
    }
}

bool XpertRequestResultXmlExport::exportCycleData(const Core::CycleData& _cycleData, Common::XmlNode& _cycleDatasNode)
{
    Tucuxi::Common::XmlNode cycleData = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "cycleData");
    _cycleDatasNode.addChild(cycleData);

    addNode(cycleData, "start", dateTimeToXmlString(_cycleData.m_start));
    addNode(cycleData, "end", dateTimeToXmlString(_cycleData.m_end));
    addNode(cycleData, "unit", _cycleData.m_unit.toString());

    // Concatenate the times.
    stringstream timesStream;
    for (size_t i = 0; i < _cycleData.m_times[0].size(); i++) {
        timesStream << std::to_string(_cycleData.m_times[0][i]);
        if (i != _cycleData.m_times[0].size() - 1) {
            timesStream << ',';
        }
    }
    addNode(cycleData, "times", timesStream.str());

    // Concatenate the points.
    stringstream pointsStream;
    for (size_t i = 0; i < _cycleData.m_concentrations[0].size(); i++) {
        pointsStream << std::to_string(_cycleData.m_concentrations[0][i]);
        if (i != _cycleData.m_concentrations[0].size() - 1) {
            pointsStream << ',';
        }
    }
    addNode(cycleData, "values", pointsStream.str());

    return true;
}

void XpertRequestResultXmlExport::exportParameters(
        const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode)
{
    // <parameters>
    Tucuxi::Common::XmlNode parametersNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "parameters");
    _rootNode.addChild(parametersNode);

    // We always have typical[0] and apriori[1]. But aposteriori[2] is not certain.
    vector<std::string> parametersTypeNodeName{"typical", "apriori", "aposteriori"};

    // For each parameters type.
    for (size_t type = 0; type < _xpertRequestResult.getParameters().size(); ++type) {

        //   <typical> / <apriori> / <aposteriori>
        Tucuxi::Common::XmlNode parametersTypeNode =
                m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, parametersTypeNodeName[type]);
        parametersNode.addChild(parametersTypeNode);

        // For each parameter of the given parameters set
        for (const Core::ParameterValue& parameter : _xpertRequestResult.getParameters()[type]) {

            //      <parameter>
            Tucuxi::Common::XmlNode parameterNode =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "parameter");
            parametersTypeNode.addChild(parameterNode);

            //                  <id>
            addNode(parameterNode, "id", parameter.m_parameterId);

            //                  <value>
            addNode(parameterNode, "value", parameter.m_value);
        }
    }
}

void XpertRequestResultXmlExport::exportStatistics(
        const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode)
{
    // <statistics>
    Tucuxi::Common::XmlNode statisticsNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "statistics");
    _rootNode.addChild(statisticsNode);

    // Extract statistics
    double auc24 = -1.0;
    double peak = -1.0;
    double residual = -1.0;
    Tucuxi::Common::DateTime date;

    _xpertRequestResult.getCycleStats().getStatistic(0, Tucuxi::Core::CycleStatisticType::AUC24).getValue(date, auc24);
    _xpertRequestResult.getCycleStats().getStatistic(0, Tucuxi::Core::CycleStatisticType::Peak).getValue(date, peak);
    _xpertRequestResult.getCycleStats()
            .getStatistic(0, Tucuxi::Core::CycleStatisticType::Residual)
            .getValue(date, residual);

    //   <auc24>
    addNode(statisticsNode, "auc24", Common::Utils::varToString(auc24));

    //   <peak>
    addNode(statisticsNode, "peak", Common::Utils::varToString(peak));

    //   <residual>
    addNode(statisticsNode, "residual", Common::Utils::varToString(residual));
}

void XpertRequestResultXmlExport::exportComputationCovariates(
        const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode)
{
    // <computationCovariates>
    Tucuxi::Common::XmlNode computationCovariatesNode =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "computationCovariates");
    _rootNode.addChild(computationCovariatesNode);

    // Get the covariates from the first adjsutment and its first cycle data.
    const vector<Core::CovariateValue>& computationCovariates =
            _xpertRequestResult.getAdjustmentData()->getAdjustments().front().getData().front().m_covariates;

    // Extract the covariates
    for (const Core::CovariateValue& covariateValue : computationCovariates) {

        //      <computationCovariate>
        Tucuxi::Common::XmlNode computationCovariateNode =
                m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "computationCovariate");
        computationCovariatesNode.addChild(computationCovariateNode);

        //          <id>
        addNode(computationCovariateNode, "id", covariateValue.m_covariateId);

        //          <value>
        addNode(computationCovariateNode, "value", std::to_string(covariateValue.m_value));
    }
}

} // namespace Xpert
} // namespace Tucuxi
