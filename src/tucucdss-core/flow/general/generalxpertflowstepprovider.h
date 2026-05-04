// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef GENERALXPERTFLOWSTEPPROVIDER_H
#define GENERALXPERTFLOWSTEPPROVIDER_H

#include <memory>

#include "tucucdss-core/flow/abstract/abstractxpertflowstepprovider.h"

namespace Tucuxi {
namespace Xpert {

/// \brief General provider of xpert flow step (generically for the drugs).
///        It provides instances of:
///        - 1) CovariateValidatorAndModelSelector
///        - 2) DoseValidator
///        - 3) SampleValidator
///        - 4) DateValidator
///        - 5) TargetValidator
///        - 6) AdjustmentTraitCreator
///        - 7) RequestExecutor
///        - 8) ReportPrinter
///        - 9) PreAdjustmentDataCollector
///
class GeneralXpertFlowStepProvider : public AbstractXpertFlowStepProvider
{
public:
    /// \brief Constructor. Set the general flow steps (mainly generic/non-drug specific).
    GeneralXpertFlowStepProvider();

    /// \brief Get the step responsible for covariate validation and drug model selection.
    /// \return An instance of CovariateValidatorAndModelSelector.
    [[nodiscard]] AbstractXpertFlowStep* getCovariateValidatorAndModelSelector() const override;

    [[nodiscard]] AbstractXpertFlowStep* getCovariateChecker() const override;

    [[nodiscard]] AbstractXpertFlowStep* getJustificationCreator() const override;

    [[nodiscard]] AbstractXpertFlowStep* getTADExporter() const override;

    /// \brief Get the step responsible for doses validation.
    /// \return An instance of DoseValidator.
    [[nodiscard]] AbstractXpertFlowStep* getDoseValidator() const override;

    /// \brief Get the step responsible for sample validation.
    /// \return An instance of SampleValidator.
    [[nodiscard]] AbstractXpertFlowStep* getSampleValidator() const override;

    /// \brief Get the step responsible for date validation.
    /// \return An instance of DateValidator.
    [[nodiscard]] AbstractXpertFlowStep* getDateValidator() const override;

    /// \brief Get the step responsible for target validation.
    /// \return An instance of TargetValidator.
    [[nodiscard]] AbstractXpertFlowStep* getTargetValidator() const override;

    /// \brief Get the step responsible for adjustment trait creation.
    /// \return An instance of AdjustmentTraitCreator.
    [[nodiscard]] AbstractXpertFlowStep* getAdjustmentTraitCreator() const override;

    /// \brief Get the step responsible for request execution.
    /// \return An instance of RequestExecutor.
    [[nodiscard]] AbstractXpertFlowStep* getRequestExecutor() const override;

    /// \brief Get the step responsible for report generation.
    /// \return An instance of ReportPrinter.
    [[nodiscard]] AbstractXpertFlowStep* getReportPrinter() const override;

    /// \brief Get the step responsible for computing pre-adjustment data.
    /// \return An instance of PreAdjustmentDataCollector.
    [[nodiscard]] AbstractXpertFlowStep* getPADCollector() const override;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // GENERALXPERTFLOWSTEPPROVIDER_H
