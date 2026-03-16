// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef XPERTREQUESTRESULTXMLEXPORT_H
#define XPERTREQUESTRESULTXMLEXPORT_H

#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"

#include "tucuquery/computingqueryresponsexmlexport.h"

#include "tucucdss-core/exporter/abstractxpertrequestresultexport.h"
#include "tucucdss-core/query/admindata.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class exports an XpertRequestResult in XML.
class XpertRequestResultXmlExport :
    public AbstractXpertRequestResultExport,
    protected Query::ComputingQueryResponseXmlExport
{
public:
    /// \brief Export the result of the xpertRequest to a file. The export may fail. In this
    ///        case, the XpertRequestResult error message is set.
    /// \param _xpertRequestResult Result of the xpertRequest to export.
    void exportToFile(XpertRequestResult& _xpertRequestResult) override;

protected:
    /// \brief Transform the given XpertRequestResult into an XML string.
    /// \param _xpertRequestResult Result of the xpertRequest to transform.
    /// \param _xmlString The resulting xml string.
    void makeXmlString(const XpertRequestResult& _xpertRequestResult, std::string& _xmlString);

    /// \brief Create and append the "introduction" xml nodes: computation time, drugID, last dose and drug model id.
    /// \param _xpertRequestResult XpertRquestResult to get the information.
    /// \param _rootNode Root node where to add the created nodes.
    void exportDrugIntro(const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode);

    /// \brief Create and append the admin data.
    /// \param _admin AdminData to export.
    /// \param _rootNode  Root node where to add the created nodes.
    void exportAdminData(const AdminData* _admin, Common::XmlNode& _rootNode);

    /// \brief Create and append a patient/mandator node to an admin node.
    /// \param _fullPerson FullPerson information to export.
    /// \param _adminNode Admin node where to add the created node.
    /// \param _nodeName Name of the node to add. (Expect: "patient" or "mandator").
    void exportFullPersonData(
            const FullPersonData* _fullPerson, Common::XmlNode& _adminNode, const std::string& _nodeName);

    /// \brief Create and append a person node to a patient/mandator node.
    ///        The identifier and the title are not exported if they are empty.
    /// \param _person Person data to export.
    /// \param _patientMandatorNode Patient/mandator node where to add the created node.
    void exportPersonData(const PersonData& _person, Common::XmlNode& _patientMandatorNode);

    /// \brief Create and append an institude node to a patient/mandator node.
    ///        The identifier is not exported if it is empty.
    /// \param _institute Institute data to export.
    /// \param _patientMandatorNode Patient/mandator node where to add the created node.
    void exportInstituteData(const InstituteData* _institute, Common::XmlNode& _patientMandatorNode);

    /// \brief Create and append an address node to a person/institute node.
    ///        The state and the country are not exported if they are empty.
    /// \param _address Address data to export.
    /// \param _personInstituteNode Person/institute node where to add the created node.
    void exportAddressData(const AddressData* _address, Common::XmlNode& _personInstituteNode);

    /// \brief Create and append a phone node to a person/institute node.
    ///        The type is not exported if it is empty.
    /// \param _phone Phone data to export.
    /// \param _personInstituteNode Person/institute node where to add the created node.
    void exportPhoneData(const PhoneData* _phone, Common::XmlNode& _personInstituteNode);

    /// \brief Create and append an email node to a person/institute node.
    ///        The type is not exported if it is empty.
    /// \param _email Email data to export.
    /// \param _personInstituteNode Person/institute node where to add the created node.
    void exportEmailData(const EmailData* _email, Common::XmlNode& _personInstituteNode);

    /// \brief Create and append a clinicalDatas node to an admin node.
    /// \param _clinicalDatas Clinical data to export.
    /// \param _adminNode Admin node where to add the created node.
    void exportClinicalDatas(const ClinicalDatas* _clinicalDatas, Common::XmlNode& _adminNode);

    /// \brief Create and append the covariates validation results nodes to the root node.
    /// \param _covariateResults Covariate validation results to export.
    /// \param _rootNode Root node where to add the created nodes.
    void exportCovariateResults(
            const std::vector<CovariateValidationResult>& _covariateResults, Common::XmlNode& _rootNode);

    /// \brief Create and append the treatment node to the root node.
    /// \param _treatment Treatment to export.
    /// \param _rootNode Root node where to add the created node.
    void exportTreatment(const Core::DrugTreatment* _treatment, Common::XmlNode& _rootNode);

    /// \brief Create and append a dose to a parent node.
    ///        Override the inherited method to include the single dose warning.
    /// \param _dosage Single dose to export.
    /// \param _parentNode Node where to add the created node.
    void exportDose(const Core::SingleDose& _dosage, Common::XmlNode& _parentNode) override;

    /// \brief Create and append the sample validation results nodes to the root node.
    /// \param _sampleResults Sample validation results to export.
    /// \param _rootNode Root node where to add the created nodes.
    void exportSampleResults(const std::vector<SampleValidationResult>& _sampleResults, Common::XmlNode& _rootNode);

    /// \brief Create and append the adjustment data node to the root node.
    /// \param _adjustmentData Adjustment data to export.
    /// \param _rootNode Root node where to add the created node.
    void exportAdjustmentData(const Core::AdjustmentData* _adjustmentData, Common::XmlNode& _rootNode);

    /// \brief Create and append a cycleData node to a cycleDatas node of an adjustment.
    ///        Override inherited method to export only the needed information and not
    ///        the covariates, the statistics and the parameters.
    /// \param _cycleData Cycle data to export.
    /// \param _cycleDatasNode CycleDatas node where to add the created node.
    /// \return True.
    bool exportCycleData(const Core::CycleData& _cycleData, Common::XmlNode& _cycleDatasNode) override;

    /// \brief Create and append the parameter values to the root node.
    ///        It creates the parameters for typical and a priori types. If possible, it makes a posteriori.
    /// \param _xpertRequestResult XpertRequestResult containing the parameters to export.
    /// \param _rootNode Root node where to add the created nodes.
    void exportParameters(const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode);

    /// \brief Create and append the statistics at steady state nodes to the root node.
    /// \param _xpertRequestResult XpertRequestResult containing the statistics at steady state to export.
    /// \param _rootNode Root node where to add the created nodes.
    void exportStatistics(const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode);

    /// \brief Create and append the covariates used during the computation nodes to the root node.
    /// \param _xpertRequestResult XpertRequestResult containing the covariates used during computation to export.
    /// \param _rootNode Root node where to add the created nodes.
    void exportComputationCovariates(const XpertRequestResult& _xpertRequestResult, Common::XmlNode& _rootNode);

    /// \brief For a given validation result object, create and append a warning node to a parent node.
    /// \param _validationResult Validation result to export.
    /// \param _parentNode Parent node where to add the created node.
    template<typename T>
    void exportWarning(const AbstractValidationResult<T>& _validationResult, Common::XmlNode& _parentNode)
    {

        if (!_validationResult.getWarning().empty()) {
            Common::XmlNode warningNode =
                    m_xmlDocument.createNode(Common::EXmlNodeType::Element, "warning", _validationResult.getWarning());
            auto levelAttribute =
                    m_xmlDocument.createAttribute("level", warningLevelToString(_validationResult.getWarningLevel()));
            warningNode.addAttribute(levelAttribute);
            _parentNode.addChild(warningNode);
        }
    }

    /// \brief We keep a reference on the XML document variable to create XML nodes
    ///        in any method without having to pass it as an argument everywhere.
    Common::XmlDocument m_xmlDocument;

    /// \brief We need to keep a reference on the xpertRequest result
    ///        to retreive the dose validation results map,
    ///        the computation time and the output language.
    const XpertRequestResult* m_xpertRequestResultInUse;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // XPERTREQUESTRESULTXMLEXPORT_H
