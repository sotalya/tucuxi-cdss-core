// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include "tucucdss-core/query/admindata.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

/**************************************************
 |                   AddressData                  |
 **************************************************/

AddressData::AddressData(
        std::string _street, std::string _postCode, std::string _city, std::string _state, std::string _country)
    : m_street(std::move(_street)), m_postalCode(std::move(_postCode)), m_city(std::move(_city)),
      m_state(std::move(_state)), m_country(std::move(_country))
{
}

const std::string& AddressData::getStreet() const
{
    return m_street;
}

const std::string& AddressData::getPostalCode() const
{
    return m_postalCode;
}

const std::string& AddressData::getCity() const
{
    return m_city;
}

const std::string& AddressData::getState() const
{
    return m_state;
}

const std::string& AddressData::getCountry() const
{
    return m_country;
}

/**************************************************
 |                    PhoneData                   |
 **************************************************/

PhoneData::PhoneData(std::string _number, std::string _type) : m_number(std::move(_number)), m_type(std::move(_type)) {}

const std::string& PhoneData::getNumber() const
{
    return m_number;
}

const std::string& PhoneData::getType() const
{
    return m_type;
}

/**************************************************
 |                   EmailData                    |
 **************************************************/

EmailData::EmailData(std::string _address, std::string _type) : m_address(std::move(_address)), m_type(std::move(_type))
{
}

const std::string& EmailData::getAddress() const
{
    return m_address;
}

const std::string& EmailData::getType() const
{
    return m_type;
}

/**************************************************
 |                  PersonData                    |
 **************************************************/

PersonData::PersonData(
        std::string _id,
        std::string _title,
        std::string _firstName,
        std::string _lastName,
        unique_ptr<AddressData> _address,
        unique_ptr<PhoneData> _phone,
        unique_ptr<EmailData> _email,
        std::optional<std::string> _birthdate)
    : m_id(std::move(_id)), m_title(std::move(_title)), m_firstName(std::move(_firstName)),
      m_lastName(std::move(_lastName)), m_address(std::move(_address)), m_phone(std::move(_phone)),
      m_email(std::move(_email)), m_birthdate(std::move(_birthdate))
{
}

const std::string& PersonData::getId() const
{
    return m_id;
}

const std::string& PersonData::getTitle() const
{
    return m_title;
}

const std::string& PersonData::getFirstName() const
{
    return m_firstName;
}

const std::string& PersonData::getLastName() const
{
    return m_lastName;
}

optional<std::string> PersonData::getBirthdate() const
{
    return m_birthdate;
}

const AddressData* PersonData::getAddress() const
{
    return m_address.get();
}

const PhoneData* PersonData::getPhone() const
{
    return m_phone.get();
}

const EmailData* PersonData::getEmail() const
{
    return m_email.get();
}

/**************************************************
 |                InstituteData                   |
 **************************************************/

InstituteData::InstituteData(
        std::string _id,
        std::string _name,
        unique_ptr<AddressData> _address,
        unique_ptr<PhoneData> _phone,
        unique_ptr<EmailData> _email)
    : m_id(std::move(_id)), m_name(std::move(_name)), m_address(std::move(_address)), m_phone(std::move(_phone)),
      m_email(std::move(_email))
{
}

const std::string& InstituteData::getId() const
{
    return m_id;
}

const std::string& InstituteData::getName() const
{
    return m_name;
}

const AddressData* InstituteData::getAddress() const
{
    return m_address.get();
}

const PhoneData* InstituteData::getPhone() const
{
    return m_phone.get();
}

const EmailData* InstituteData::getEmail() const
{
    return m_email.get();
}

/**************************************************
 |                   FullPersonData               |
 **************************************************/

FullPersonData::FullPersonData(unique_ptr<PersonData> _person, unique_ptr<InstituteData> _institute)
    : m_person(std::move(_person)), m_institute(std::move(_institute))
{
}

const PersonData& FullPersonData::getPerson() const
{
    return *m_person;
}

const InstituteData* FullPersonData::getInstitute() const
{
    return m_institute.get();
}

/**************************************************
 |                 ClinicalDatas                   |
 **************************************************/

ClinicalDatas::ClinicalDatas(const map<std::string, std::string>& _data) : m_data(_data) {}

const map<std::string, std::string>& ClinicalDatas::getData() const
{
    return m_data;
}

/**************************************************
 |                   AdminData                    |
 **************************************************/

AdminData::AdminData(
        unique_ptr<FullPersonData> _mandator,
        unique_ptr<FullPersonData> _patient,
        unique_ptr<ClinicalDatas> _clinicalDatas)
    : m_mandator(std::move(_mandator)), m_patient(std::move(_patient)), m_clinicalDatas(std::move(_clinicalDatas))
{
}

const FullPersonData* AdminData::getMandator() const
{
    return m_mandator.get();
}

const FullPersonData* AdminData::getPatient() const
{
    return m_patient.get();
}

const ClinicalDatas* AdminData::getClinicalDatas() const
{
    return m_clinicalDatas.get();
}

} // namespace Xpert
} // namespace Tucuxi
