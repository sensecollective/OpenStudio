/**********************************************************************
 *  Copyright (c) 2008-2016, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#include "ElectricLoadCenterStorageSimple.hpp"
#include "ElectricLoadCenterStorageSimple_Impl.hpp"

// TODO: Check the following class names against object getters and setters.
#include "Schedule.hpp"
#include "Schedule_Impl.hpp"
#include "ThermalZone.hpp"
#include "ThermalZone_Impl.hpp"
#include "Model.hpp"
#include "Model_Impl.hpp"

#include "../../model/ScheduleTypeLimits.hpp"
#include "../../model/ScheduleTypeRegistry.hpp"

#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/OS_ElectricLoadCenter_Storage_Simple_FieldEnums.hxx>

#include "../utilities/units/Unit.hpp"
#include "../utilities/core/Assert.hpp"

namespace openstudio {
namespace model {

namespace detail {

  ElectricLoadCenterStorageSimple_Impl::ElectricLoadCenterStorageSimple_Impl(const IdfObject& idfObject,
                                                                             Model_Impl* model,
                                                                             bool keepHandle)
    : ElectricalStorage_Impl(idfObject,model,keepHandle)
  {
    OS_ASSERT(idfObject.iddObject().type() == ElectricLoadCenterStorageSimple::iddObjectType());
  }

  ElectricLoadCenterStorageSimple_Impl::ElectricLoadCenterStorageSimple_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                                                                             Model_Impl* model,
                                                                             bool keepHandle)
    : ElectricalStorage_Impl(other,model,keepHandle)
  {
    OS_ASSERT(other.iddObject().type() == ElectricLoadCenterStorageSimple::iddObjectType());
  }

  ElectricLoadCenterStorageSimple_Impl::ElectricLoadCenterStorageSimple_Impl(const ElectricLoadCenterStorageSimple_Impl& other,
                                                                             Model_Impl* model,
                                                                             bool keepHandle)
    : ElectricalStorage_Impl(other,model,keepHandle)
  {}

  const std::vector<std::string>& ElectricLoadCenterStorageSimple_Impl::outputVariableNames() const
  {
    static std::vector<std::string> result;
    if (result.empty()){
      result.push_back("Electric Storage Simple Charge State");
      result.push_back("Electric Storage Charge Power");
      result.push_back("Electric Storage Charge Energy");
      result.push_back("Electric Storage Production Decrement Energy");
      result.push_back("Electric Storage Discharge Power");
      result.push_back("Electric Storage Discharge Energy");
      result.push_back("Electric Storage Thermal Loss Rate");
      result.push_back("Electric Storage Thermal Loss Energy");
    }
    return result;
  }

  IddObjectType ElectricLoadCenterStorageSimple_Impl::iddObjectType() const {
    return ElectricLoadCenterStorageSimple::iddObjectType();
  }

  std::vector<ScheduleTypeKey> ElectricLoadCenterStorageSimple_Impl::getScheduleTypeKeys(const Schedule& schedule) const
  {
    std::vector<ScheduleTypeKey> result;
    UnsignedVector fieldIndices = getSourceIndices(schedule.handle());
    UnsignedVector::const_iterator b(fieldIndices.begin()), e(fieldIndices.end());
    if (std::find(b,e,OS_ElectricLoadCenter_Storage_SimpleFields::AvailabilityScheduleName) != e)
    {
      result.push_back(ScheduleTypeKey("ElectricLoadCenterStorageSimple","Availability"));
    }
    return result;
  }
  
  //@}
  /** @name Getters */
  //@{
  
  // Included in parent class
  // boost::optional<ElectricLoadCenterDistribution> ElectricLoadCenterStorageSimple_Impl::electricLoadCenterDistribution() const
  // {
    // boost::optional<ElectricLoadCenterDistribution> result;
    // for (auto list : getObject<ModelObject>().getModelObjectSources<ModelObjectList>(ModelObjectList::iddObjectType())){
      // auto elcds = list.getModelObjectSources<ElectricLoadCenterDistribution>(ElectricLoadCenterDistribution::iddObjectType());
      // if (elcds.empty()){
        // error
      // } else if (elcds.size() == 1u){
        // return elcds[0];
      // }else{
        // error
      // }
    // }
    // return boost::none;
  // }
  
  /*
  boost::optional<ElectricLoadCenterDistribution> ElectricLoadCenterStorageSimple_Impl::electricLoadCenterDistribution const {

    boost::optional<ElectricLoadCenterDistribution> value;
    for ( const ElectricLoadCenterDistribution& elcd : this->model().getConcreteModelObjects<ElectricLoadCenterDistribution>() )
    {
      if ( boost::optional<ElectricalStorage> elecStor = mchp.electricalStorage() )
      {
        if (elecStor->handle() == this->handle())
        {
          value = elcd;
        }
      }
    }
    return value;

  }*/

  boost::optional<Schedule> ElectricLoadCenterStorageSimple_Impl::optionalAvailabilitySchedule() const {
    return getObject<ModelObject>().getModelObjectTarget<Schedule>(OS_ElectricLoadCenter_Storage_SimpleFields::AvailabilityScheduleName);
  }
  
  Schedule ElectricLoadCenterStorageSimple_Impl::availabilitySchedule() const {
    boost::optional<Schedule> value = optionalAvailabilitySchedule();
    if (!value) {
      //LOG_AND_THROW(briefDescription() << " does not have an Availability Schedule attached.");
      // I'm choosing to just return alwaysOnDiscreteSchedule
      return this->model().alwaysOnDiscreteSchedule();
    }
    return value.get();
  }

  bool ElectricLoadCenterStorageSimple_Impl::isAvailabilityScheduleDefaulted() const {
    return isEmpty(OS_ElectricLoadCenter_Storage_SimpleFields::AvailabilityScheduleName);
  }
  
  // Optional thermal Zone for heat gains  
  boost::optional<ThermalZone> ElectricLoadCenterStorageSimple_Impl::thermalZone() const {
    return getObject<ModelObject>().getModelObjectTarget<ThermalZone>(OS_ElectricLoadCenter_Storage_SimpleFields::ZoneName);
  }

  // Radiative fraction for zone heat gains, defaults to 0, and not used if thermalZone above isn't used
  double ElectricLoadCenterStorageSimple_Impl::radiativeFractionforZoneHeatGains() const {
    boost::optional<double> value = getDouble(OS_ElectricLoadCenter_Storage_SimpleFields::RadiativeFractionforZoneHeatGains,true);
    OS_ASSERT(value);
    return value.get();
  }

  bool ElectricLoadCenterStorageSimple_Impl::isRadiativeFractionforZoneHeatGainsDefaulted() const {
    return isEmpty(OS_ElectricLoadCenter_Storage_SimpleFields::RadiativeFractionforZoneHeatGains);
  }

  // Nominal Efficiency for Charging for zone heat gains, has a default in IDD
  double ElectricLoadCenterStorageSimple_Impl::nominalEnergeticEfficiencyforCharging() const {
    boost::optional<double> value = getDouble(OS_ElectricLoadCenter_Storage_SimpleFields::NominalEnergeticEfficiencyforCharging,true);
    OS_ASSERT(value);
    return value.get();
  }

  bool ElectricLoadCenterStorageSimple_Impl::isNominalEnergeticEfficiencyforChargingDefaulted() const {
    return isEmpty(OS_ElectricLoadCenter_Storage_SimpleFields::NominalEnergeticEfficiencyforCharging);
  }

  // Nominal Efficiency for Discharging for zone heat gains, has a default in IDD
  double ElectricLoadCenterStorageSimple_Impl::nominalDischargingEnergeticEfficiency() const {
    boost::optional<double> value = getDouble(OS_ElectricLoadCenter_Storage_SimpleFields::NominalDischargingEnergeticEfficiency,true);
    OS_ASSERT(value);
    return value.get();
  }

  bool ElectricLoadCenterStorageSimple_Impl::isNominalDischargingEnergeticEfficiencyDefaulted() const {
    return isEmpty(OS_ElectricLoadCenter_Storage_SimpleFields::NominalDischargingEnergeticEfficiency);
  }
  
  // Maximum Storage Capacity, required, no default
  double ElectricLoadCenterStorageSimple_Impl::maximumStorageCapacity() const {
    boost::optional<double> value = getDouble(OS_ElectricLoadCenter_Storage_SimpleFields::MaximumStorageCapacity,true);
    OS_ASSERT(value);
    return value.get();
  }
  
  // Maximum Rate of Discharge, required, no default
  double ElectricLoadCenterStorageSimple_Impl::maximumPowerforDischarging() const {
    boost::optional<double> value = getDouble(OS_ElectricLoadCenter_Storage_SimpleFields::MaximumPowerforDischarging,true);
    OS_ASSERT(value);
    return value.get();
  }

  // Maximum Rate of Charge, required, no default
  double ElectricLoadCenterStorageSimple_Impl::maximumPowerforCharging() const {
    boost::optional<double> value = getDouble(OS_ElectricLoadCenter_Storage_SimpleFields::MaximumPowerforCharging,true);
    OS_ASSERT(value);
    return value.get();
  }

  // Initial state of charge, if empty it will default to half of the maximum capacity
  double ElectricLoadCenterStorageSimple_Impl::initialStateofCharge() const {
    boost::optional<double> optIniStateCharge = getDouble(OS_ElectricLoadCenter_Storage_SimpleFields::InitialStateofCharge,true);
    // if it's set
    if (optIniStateCharge) {
      // Get it and return
      return optIniStateCharge.get();
    } else {
      // We Default it to half the maximum capacity
      double maxCap = maximumStorageCapacity();
      return maxCap / 2.0;
    }
  }
  
  bool ElectricLoadCenterStorageSimple_Impl::isInitialStateofChargeDefaulted() const {
    return isEmpty(OS_ElectricLoadCenter_Storage_SimpleFields::InitialStateofCharge);
  }
  
  //@}
  /** @name Setters */
  //@{

  bool ElectricLoadCenterStorageSimple_Impl::setAvailabilitySchedule(Schedule& schedule) {
    bool result = setSchedule(OS_ElectricLoadCenter_Storage_SimpleFields::AvailabilityScheduleName,
                              "ElectricLoadCenterStorageSimple",
                              "Availability",
                              schedule);
    return result;
  }

  void ElectricLoadCenterStorageSimple_Impl::resetAvailabilitySchedule() {
    bool result = setString(OS_ElectricLoadCenter_Storage_SimpleFields::AvailabilityScheduleName, "");
    OS_ASSERT(result);
  }

  // Set the thermal Zone for heat gains
  bool ElectricLoadCenterStorageSimple_Impl::setThermalZone(const ThermalZone& thermalZone) {
    return setPointer(OS_ElectricLoadCenter_Storage_SimpleFields::ZoneName, thermalZone.handle());
  }
  
  // Reset the zone (leave blank)
  void ElectricLoadCenterStorageSimple_Impl::resetThermalZone() {
    bool result = setString(OS_ElectricLoadCenter_Storage_SimpleFields::ZoneName, "");
    OS_ASSERT(result);
  }
  

  bool ElectricLoadCenterStorageSimple_Impl::setRadiativeFractionforZoneHeatGains(double radiativeFractionforZoneHeatGains) {
    bool result = setDouble(OS_ElectricLoadCenter_Storage_SimpleFields::RadiativeFractionforZoneHeatGains, radiativeFractionforZoneHeatGains);
    return result;
  }

  void ElectricLoadCenterStorageSimple_Impl::resetRadiativeFractionforZoneHeatGains() {
    bool result = setString(OS_ElectricLoadCenter_Storage_SimpleFields::RadiativeFractionforZoneHeatGains, "");
    OS_ASSERT(result);
  }

  bool ElectricLoadCenterStorageSimple_Impl::setNominalEnergeticEfficiencyforCharging(double nominalEnergeticEfficiencyforCharging) {
    bool result = setDouble(OS_ElectricLoadCenter_Storage_SimpleFields::NominalEnergeticEfficiencyforCharging, nominalEnergeticEfficiencyforCharging);
    return result;
  }

  void ElectricLoadCenterStorageSimple_Impl::resetNominalEnergeticEfficiencyforCharging() {
    bool result = setString(OS_ElectricLoadCenter_Storage_SimpleFields::NominalEnergeticEfficiencyforCharging, "");
    OS_ASSERT(result);
  }

  bool ElectricLoadCenterStorageSimple_Impl::setNominalDischargingEnergeticEfficiency(double nominalDischargingEnergeticEfficiency) {
    bool result = setDouble(OS_ElectricLoadCenter_Storage_SimpleFields::NominalDischargingEnergeticEfficiency, nominalDischargingEnergeticEfficiency);
    return result;
  }

  void ElectricLoadCenterStorageSimple_Impl::resetNominalDischargingEnergeticEfficiency() {
    bool result = setString(OS_ElectricLoadCenter_Storage_SimpleFields::NominalDischargingEnergeticEfficiency, "");
    OS_ASSERT(result);
  }

  bool ElectricLoadCenterStorageSimple_Impl::setMaximumStorageCapacity(double maximumStorageCapacity) {
    bool result = setDouble(OS_ElectricLoadCenter_Storage_SimpleFields::MaximumStorageCapacity, maximumStorageCapacity);
    return result;
  }

  bool ElectricLoadCenterStorageSimple_Impl::setMaximumPowerforDischarging(double maximumPowerforDischarging) {
    bool result = setDouble(OS_ElectricLoadCenter_Storage_SimpleFields::MaximumPowerforDischarging, maximumPowerforDischarging);
    return result;
  }

  bool ElectricLoadCenterStorageSimple_Impl::setMaximumPowerforCharging(double maximumPowerforCharging) {
    bool result = setDouble(OS_ElectricLoadCenter_Storage_SimpleFields::MaximumPowerforCharging, maximumPowerforCharging);
    return result;
  }

  bool ElectricLoadCenterStorageSimple_Impl::setInitialStateofCharge(double value) {
    bool result = setDouble(OS_ElectricLoadCenter_Storage_SimpleFields::InitialStateofCharge, value);
    return result;
  }

  void ElectricLoadCenterStorageSimple_Impl::resetInitialStateofCharge() {
    bool result = setString(OS_ElectricLoadCenter_Storage_SimpleFields::InitialStateofCharge, "");
    OS_ASSERT(result);
  }

} // detail

/* Constructor. Defaults the availabilitySchedule to alwaysOnDiscreteSchedule,
   Defaults efficiencies for charge and discharge to 0.8
   maximum storage capacity to 1.0E13
   Maximum Power for Charge/Discharge to 1.0E6
   Reference: EnergyPlus example files (v8.5, LrgOff_GridStorageDemandLeveling */
ElectricLoadCenterStorageSimple::ElectricLoadCenterStorageSimple(const Model& model)
  : ElectricalStorage(ElectricLoadCenterStorageSimple::iddObjectType(),model)
{
  OS_ASSERT(getImpl<detail::ElectricLoadCenterStorageSimple_Impl>());

  Schedule schedule = model.alwaysOnDiscreteSchedule();

  setAvailabilitySchedule(schedule);
  
  setNominalEnergeticEfficiencyforCharging(0.8);
  setNominalDischargingEnergeticEfficiency(0.8);
  setMaximumStorageCapacity(1.0E13);
  setMaximumPowerforDischarging(1.0E6);
  setMaximumPowerforCharging(1.0E6);
  
}

IddObjectType ElectricLoadCenterStorageSimple::iddObjectType() {
  return IddObjectType(IddObjectType::OS_ElectricLoadCenter_Storage_Simple);
}

// Convenience method to return the electricalLoadCenter on which it's assigned (optional)
// Included in parent
// boost::optional<ElectricLoadCenterDistribution> ElectricLoadCenterStorageSimple::electricLoadCenterDistribution() const
// {
  // return getImpl<detail::Generator_Impl>()->electricLoadCenterDistribution();
// }

Schedule ElectricLoadCenterStorageSimple::availabilitySchedule() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->availabilitySchedule();
}

bool ElectricLoadCenterStorageSimple::isAvailabilityScheduleDefaulted() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->isAvailabilityScheduleDefaulted();
}

// Included in parent
// boost::optional<ThermalZone> ElectricLoadCenterStorageSimple::thermalZone() const {
  // return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->thermalZone();
// }

double ElectricLoadCenterStorageSimple::radiativeFractionforZoneHeatGains() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->radiativeFractionforZoneHeatGains();
}

bool ElectricLoadCenterStorageSimple::isRadiativeFractionforZoneHeatGainsDefaulted() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->isRadiativeFractionforZoneHeatGainsDefaulted();
}

double ElectricLoadCenterStorageSimple::nominalEnergeticEfficiencyforCharging() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->nominalEnergeticEfficiencyforCharging();
}

bool ElectricLoadCenterStorageSimple::isNominalEnergeticEfficiencyforChargingDefaulted() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->isNominalEnergeticEfficiencyforChargingDefaulted();
}

double ElectricLoadCenterStorageSimple::nominalDischargingEnergeticEfficiency() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->nominalDischargingEnergeticEfficiency();
}

bool ElectricLoadCenterStorageSimple::isNominalDischargingEnergeticEfficiencyDefaulted() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->isNominalDischargingEnergeticEfficiencyDefaulted();
}

double ElectricLoadCenterStorageSimple::maximumStorageCapacity() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->maximumStorageCapacity();
}

double ElectricLoadCenterStorageSimple::maximumPowerforDischarging() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->maximumPowerforDischarging();
}

double ElectricLoadCenterStorageSimple::maximumPowerforCharging() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->maximumPowerforCharging();
}

double ElectricLoadCenterStorageSimple::initialStateofCharge() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->initialStateofCharge();
}

bool ElectricLoadCenterStorageSimple::isInitialStateofChargeDefaulted() const {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->isInitialStateofChargeDefaulted();
}

// Setters
bool ElectricLoadCenterStorageSimple::setAvailabilitySchedule(Schedule& schedule) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setAvailabilitySchedule(schedule);
}

void ElectricLoadCenterStorageSimple::resetAvailabilitySchedule() {
  getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->resetAvailabilitySchedule();
}

// Included in parent
// bool ElectricLoadCenterStorageSimple::setThermalZone(ThermalZone& zone) {
  // return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setThermalZone(zone);
// }
// void ElectricLoadCenterStorageSimple::resetThermalZone() {
  // getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->resetThermalZone();
// }

bool ElectricLoadCenterStorageSimple::setRadiativeFractionforZoneHeatGains(double radiativeFractionforZoneHeatGains) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setRadiativeFractionforZoneHeatGains(radiativeFractionforZoneHeatGains);
}

void ElectricLoadCenterStorageSimple::resetRadiativeFractionforZoneHeatGains() {
  getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->resetRadiativeFractionforZoneHeatGains();
}

bool ElectricLoadCenterStorageSimple::setNominalEnergeticEfficiencyforCharging(double nominalEnergeticEfficiencyforCharging) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setNominalEnergeticEfficiencyforCharging(nominalEnergeticEfficiencyforCharging);
}

void ElectricLoadCenterStorageSimple::resetNominalEnergeticEfficiencyforCharging() {
  getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->resetNominalEnergeticEfficiencyforCharging();
}

bool ElectricLoadCenterStorageSimple::setNominalDischargingEnergeticEfficiency(double nominalDischargingEnergeticEfficiency) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setNominalDischargingEnergeticEfficiency(nominalDischargingEnergeticEfficiency);
}

void ElectricLoadCenterStorageSimple::resetNominalDischargingEnergeticEfficiency() {
  getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->resetNominalDischargingEnergeticEfficiency();
}

bool ElectricLoadCenterStorageSimple::setMaximumStorageCapacity(double maximumStorageCapacity) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setMaximumStorageCapacity(maximumStorageCapacity);
}

bool ElectricLoadCenterStorageSimple::setMaximumPowerforDischarging(double maximumPowerforDischarging) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setMaximumPowerforDischarging(maximumPowerforDischarging);
}

bool ElectricLoadCenterStorageSimple::setMaximumPowerforCharging(double maximumPowerforCharging) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setMaximumPowerforCharging(maximumPowerforCharging);
}

bool ElectricLoadCenterStorageSimple::setInitialStateofCharge(double initialStateofCharge) {
  return getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->setInitialStateofCharge(initialStateofCharge);
}

void ElectricLoadCenterStorageSimple::resetInitialStateofCharge() {
  getImpl<detail::ElectricLoadCenterStorageSimple_Impl>()->resetInitialStateofCharge();
}

/// @cond
ElectricLoadCenterStorageSimple::ElectricLoadCenterStorageSimple(std::shared_ptr<detail::ElectricLoadCenterStorageSimple_Impl> impl)
  : ElectricalStorage(impl)
{}
/// @endcond

} // model
} // openstudio

