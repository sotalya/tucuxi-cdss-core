// SPDX-License-Identifier: AGPL-3.0-or-later
#include "justification.h"

namespace Tucuxi {
namespace Xpert {

void Justification::setFirstDoseDate(const Common::DateTime& _date)
{
    m_firstDoseDate = _date;
}

void Justification::setFirstDoseText(const std::string& _text)
{
    m_firstDoseText = _text;
}

void Justification::setFirstDoseValue(double _value)
{
    m_firstDoseValue = _value;
}

void Justification::setSecondDoseText(const std::string& _text)
{
    m_secondDoseText = _text;
}

void Justification::setSecondDoseDate(const Common::DateTime& _date)
{
    m_secondDoseDate = _date;
}

void Justification::setJustificationType(JustificationType _type)
{
    m_justificationType = _type;
}

void Justification::setJustificationDoseSign(JustificationDoseSign _sign)
{
    m_justificationDoseSign = _sign;
}

void Justification::setJustificationExposureSign(JustificationExposureSign _sign)
{
    m_justificationExposureSign = _sign;
}

void Justification::setJustificationInterval(JustificationInterval _interval)
{
    m_justificationInterval = _interval;
}

const std::string& Justification::getFirstDoseText() const
{
    return m_firstDoseText;
}

const Common::DateTime& Justification::getFirstDoseDate() const
{
    return m_firstDoseDate;
}

double Justification::getFirstDoseValue() const
{
    return m_firstDoseValue;
}

const std::string& Justification::getSecondDoseText() const
{
    return m_secondDoseText;
}

const Common::DateTime& Justification::getSecondDoseDate() const
{
    return m_secondDoseDate;
}

JustificationType Justification::getJustificationType() const
{
    return m_justificationType;
}

JustificationDoseSign Justification::getJustificationDoseSign() const
{
    return m_justificationDoseSign;
}

JustificationExposureSign Justification::getJustificationExposureSign() const
{
    return m_justificationExposureSign;
}

JustificationInterval Justification::getJustificationInterval() const
{
    return m_justificationInterval;
}

} // namespace Xpert
} // namespace Tucuxi
