// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef ABSTRACTXPERTFLOWSTEPPROVIDER_H
#define ABSTRACTXPERTFLOWSTEPPROVIDER_H

#include <memory>

#include "tucucdss-core/flow/abstract/abstractxpertflowstep.h"

namespace Tucuxi {
namespace Xpert {

/// \brief Abstract class that must be implemented by xpert flow step providers.
///        An xpert flow step provider is an object responsible for providing
///        the execution steps for a given drug. An execution step is:
///        - 1) Validate the covariates and select drug model
///        - 2) Validate the doses
///        - 3) Validate the samples
///        - 4) Validate the dates
///        - 5) Validate the targets
///        - 6) Create the adjustment trait
///        - 7) Execute the requests
///        - 8) Print the treatment adjustment report
///        - 9) Computing the pre-adjustment data
///
///        A flow step is an implementation of the AbstractXpertFlowStep class.
///
///        When necessary, this factory system allows to be able to offer a specific
///        implementation for a drug: ImatinibXpertFlowStepProvider, RifampicinXpertFlowStepProvider...
///
class AbstractXpertFlowStepProvider
{

public:
    /// \brief Constructor. Simply set members to nullptr.
    AbstractXpertFlowStepProvider() = default;

    /// \brief Destructor.
    virtual ~AbstractXpertFlowStepProvider(){}; // LCOV_EXCL_LINE

    /// \brief Get the step responsible for covariate validation and drug model selection.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getCovariateValidatorAndModelSelector() const = 0;

    [[nodiscard]] virtual AbstractXpertFlowStep* getCovariateChecker() const = 0;

    [[nodiscard]] virtual AbstractXpertFlowStep* getJustificationCreator() const = 0;

    [[nodiscard]] virtual AbstractXpertFlowStep* getTADExporter() const = 0;

    /// \brief Get the step responsible for dose validation.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getDoseValidator() const = 0;

    /// \brief Get the step responsible for sample validation.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getSampleValidator() const = 0;

    /// \brief Get the step responsible for date validation.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getDateValidator() const = 0;

    /// \brief Get the step responsible for target validation.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getTargetValidator() const = 0;

    /// \brief Get the step responsible for adjustment trait creation.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getAdjustmentTraitCreator() const = 0;

    /// \brief Get the step responsible for request execution.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getRequestExecutor() const = 0;

    /// \brief Get the step responsible for report generation.
    /// \return The corresponding AbstractXpertFlowStep.
    [[nodiscard]] virtual AbstractXpertFlowStep* getReportPrinter() const = 0;

    /// \brief Get the step responsible for computing pre-adjustment data.
    /// \return An instance of PreAdjustmentDataCollector.
    [[nodiscard]] virtual AbstractXpertFlowStep* getPADCollector() const = 0;

protected:
    /// \brief The step responsible for covariate validation and drug model selection.
    std::unique_ptr<AbstractXpertFlowStep> m_covariateValidatorAndModelSelector{nullptr};

    std::unique_ptr<AbstractXpertFlowStep> m_covariateChecker{nullptr};

    std::unique_ptr<AbstractXpertFlowStep> m_justificationCreator{nullptr};

    std::unique_ptr<AbstractXpertFlowStep> m_tdaExporter{nullptr};

    /// \brief The step responsible for dose validation.
    std::unique_ptr<AbstractXpertFlowStep> m_doseValidator{nullptr};

    /// \brief The step responsible for sample validation.
    std::unique_ptr<AbstractXpertFlowStep> m_sampleValidator{nullptr};

    /// \brief The step responsible for date validation.
    std::unique_ptr<AbstractXpertFlowStep> m_dateValidator{nullptr};

    /// \brief The step responsible for target validation.
    std::unique_ptr<AbstractXpertFlowStep> m_targetValidator{nullptr};

    /// \brief The step responsible for adjustment trait creation.
    std::unique_ptr<AbstractXpertFlowStep> m_adjustmentTraitCreator{nullptr};

    /// \brief The step responsible for request execution.
    std::unique_ptr<AbstractXpertFlowStep> m_requestExecutor{nullptr};

    /// \brief The step responsible for report generation.
    std::unique_ptr<AbstractXpertFlowStep> m_reportPrinter{nullptr};

    /// \brief The step responsible for computing pre-adjustment data.
    std::unique_ptr<AbstractXpertFlowStep> m_padCollector{nullptr};
};

} // namespace Xpert
} // namespace Tucuxi

#endif // ABSTRACTXPERTFLOWSTEPPROVIDER_H
