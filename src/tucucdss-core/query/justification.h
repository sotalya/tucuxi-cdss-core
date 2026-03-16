// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef JUSTIFICATION_H
#define JUSTIFICATION_H

#include <string>

#include "tucucommon/datetime.h"

namespace Tucuxi {
namespace Xpert {

///
/// \brief Enumeration representing the type of justification.
///
/// SIMPLE: Represents a simple justification.
/// DOUBLE: Represents a double justification.
///
enum class JustificationType
{
    SIMPLE, ///< Simple justification
    DOUBLE  ///< Double justification
};

///
/// \brief Enumeration representing the change in dosing interval.
///
/// LOWER: The new interval is shorter than the previous one.
/// HIGHER: The new interval is longer than the previous one.
/// EQUAL: The interval remains unchanged.
/// NEW: There was no previous interval (new dosage).
///
enum class JustificationInterval
{
    LOWER,  ///< Shorter interval
    HIGHER, ///< Longer interval
    EQUAL,  ///< Unchanged interval
    NEW     ///< New interval (no previous dosage)
};

///
/// \brief Enumeration representing the sign of justification dose.
///
/// DECREASE: Represents a decrease.
/// INCREASE: Represents an increase.
/// EQUAL: Represents an equal value.
/// NEW: Represents a new value.
///
enum class JustificationDoseSign
{
    DECREASE, ///< Decrease sign
    INCREASE, ///< Increase sign
    EQUAL,    ///< Equal sign
    NEW       ///< New sign
};

///
/// \brief Enumeration representing the sign of justification exposure.
///
/// ABOVE: The past drug exposure was above target.
/// BELOW: The past drug exposure was below target.
///
enum class JustificationExposureSign
{
    ABOVE, ///< Above sign
    BELOW, ///< Below sign
    EQUAL  ///< EQUAL sign
};

///
/// \brief Class representing a justification with relevant data.
///
class Justification
{
private:
    JustificationType m_justificationType{};                 ///< Type of the justification
    JustificationDoseSign m_justificationDoseSign{};         ///< Sign of the justification
    JustificationExposureSign m_justificationExposureSign{}; ///< Sign of the exposure relative to the target
    JustificationInterval m_justificationInterval{};         ///< Change in dosing interval

    std::string m_firstDoseText;      ///< Description of the first dose
    Common::DateTime m_firstDoseDate; ///< Date of the first dose
    double m_firstDoseValue{};        ///< Value of the first dose

    std::string m_secondDoseText;      ///< Description of the second dose (if applicable)
    Common::DateTime m_secondDoseDate; ///< Date of the second dose (if applicable)

public:
    ///
    /// \brief Default constructor for Justification.
    ///
    Justification() = default;

    ///
    /// \brief Sets the text description for the first dose.
    /// \param _text The text describing the first dose.
    ///
    void setFirstDoseText(const std::string& _text);

    ///
    /// \brief Sets the date for the first dose.
    /// \param _date The date of the first dose.
    ///
    void setFirstDoseDate(const Common::DateTime& _date);

    ///
    /// \brief Sets the value for the first dose.
    /// \param _value The value of the first dose.
    ///
    void setFirstDoseValue(double _value);

    ///
    /// \brief Sets the text description for the second dose.
    /// \param _text The text describing the second dose.
    ///
    void setSecondDoseText(const std::string& _text);

    ///
    /// \brief Sets the date for the second dose.
    /// \param _date The date of the second dose.
    ///
    void setSecondDoseDate(const Common::DateTime& _date);

    ///
    /// \brief Sets the type of justification.
    /// \param _type The type of justification (SIMPLE or DOUBLE).
    ///
    void setJustificationType(JustificationType _type);

    ///
    /// \brief Sets the sign of justification.
    /// \param _sign The sign of justification (DECREASE, INCREASE, EQUAL, NEW).
    ///
    void setJustificationDoseSign(JustificationDoseSign _sign);

    ///
    /// \brief Sets the sign of justification exposure.
    /// \param _sign The sign of justification (ABOVE or BELOW).
    ///
    void setJustificationExposureSign(JustificationExposureSign _sign);

    ///
    /// \brief Sets the change in dosing interval.
    /// \param _interval The interval change (LOWER, HIGHER, EQUAL, NEW).
    ///
    void setJustificationInterval(JustificationInterval _interval);

    ///
    /// \brief Retrieves the text of the first dose.
    /// \return A constant reference to the first dose text.
    ///
    const std::string& getFirstDoseText() const;

    ///
    /// \brief Retrieves the date of the first dose.
    /// \return A constant reference to the first dose date.
    ///
    const Common::DateTime& getFirstDoseDate() const;

    ///
    /// \brief Retrieves the value of the first dose.
    /// \return The first dose value.
    ///
    double getFirstDoseValue() const;

    ///
    /// \brief Retrieves the text of the second dose.
    /// \return A constant reference to the second dose text.
    ///
    const std::string& getSecondDoseText() const;

    ///
    /// \brief Retrieves the date of the second dose.
    /// \return A constant reference to the second dose date.
    ///
    const Common::DateTime& getSecondDoseDate() const;

    ///
    /// \brief Retrieves the type of justification.
    /// \return The type of justification (SIMPLE or DOUBLE).
    ///
    JustificationType getJustificationType() const;

    ///
    /// \brief Retrieves the sign of justification.
    /// \return The sign of justification (DECREASE, INCREASE, EQUAL, NEW).
    ///
    JustificationDoseSign getJustificationDoseSign() const;

    ///
    /// \brief Retrieves the sign of justification exposure.
    /// \return The exposure sign (ABOVE, BELOW, EQUAL).
    ///
    JustificationExposureSign getJustificationExposureSign() const;

    ///
    /// \brief Retrieves the change in dosing interval.
    /// \return The interval change (LOWER, HIGHER, EQUAL, NEW).
    ///
    JustificationInterval getJustificationInterval() const;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // JUSTIFICATION_H
