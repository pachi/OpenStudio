/**********************************************************************
 *  Copyright (c) 2008-2015, Alliance for Sustainable Energy.
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

#include <model/SolarCollectorFlatPlateWater.hpp>
#include <model/SolarCollectorFlatPlateWater_Impl.hpp>

#include <model/SolarCollectorPerformanceFlatPlate.hpp>
#include <model/SolarCollectorPerformanceFlatPlate_Impl.hpp>
#include <model/Surface.hpp>
#include <model/Surface_Impl.hpp>
#include <model/ShadingSurface.hpp>
#include <model/ShadingSurface_Impl.hpp>
#include <model/Connection.hpp>
#include <model/Connection_Impl.hpp>
#include <model/ModelObject.hpp>
#include <model/ModelObject_Impl.hpp>
#include <model/PlantLoop.hpp>
#include <model/PlantLoop_Impl.hpp>
#include <model/Node.hpp>
#include <model/Model.hpp>

#include <utilities/idd/OS_SolarCollector_FlatPlate_Water_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

#include <utilities/units/Unit.hpp>

#include <utilities/core/Assert.hpp>

namespace openstudio {
namespace model {

namespace detail {

  SolarCollectorFlatPlateWater_Impl::SolarCollectorFlatPlateWater_Impl(const IdfObject& idfObject,
                                                                       Model_Impl* model,
                                                                       bool keepHandle)
    : StraightComponent_Impl(idfObject,model,keepHandle)
  {
    OS_ASSERT(idfObject.iddObject().type() == SolarCollectorFlatPlateWater::iddObjectType());
  }

  SolarCollectorFlatPlateWater_Impl::SolarCollectorFlatPlateWater_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                                                                       Model_Impl* model,
                                                                       bool keepHandle)
    : StraightComponent_Impl(other,model,keepHandle)
  {
    OS_ASSERT(other.iddObject().type() == SolarCollectorFlatPlateWater::iddObjectType());
  }

  SolarCollectorFlatPlateWater_Impl::SolarCollectorFlatPlateWater_Impl(const SolarCollectorFlatPlateWater_Impl& other,
                                                                       Model_Impl* model,
                                                                       bool keepHandle)
    : StraightComponent_Impl(other,model,keepHandle)
  {}

  ModelObject SolarCollectorFlatPlateWater_Impl::clone(Model model) const 
  {

    SolarCollectorFlatPlateWater result = StraightComponent_Impl::clone(model).cast<SolarCollectorFlatPlateWater>();
    result.setSolarCollectorPerformance(this->solarCollectorPerformance());

    // do not want to point to any surface after cloning
    result.resetSurface();

    return result;
  }

  std::vector<IdfObject> SolarCollectorFlatPlateWater_Impl::remove()
  {
    // DLM: will remove performance object due to parent/child relationship
    return StraightComponent_Impl::remove();
  }

  const std::vector<std::string>& SolarCollectorFlatPlateWater_Impl::outputVariableNames() const
  {
    static std::vector<std::string> result;
    if (result.empty()){
      result.push_back("Solar Collector Incident Angle Modifier");
      result.push_back("Solar Collector Efficiency");
      result.push_back("Solar Collector Heat Transfer Rate");
      result.push_back("Solar Collector Heat Gain Rate");
      result.push_back("Solar Collector Heat Loss Rate");
      result.push_back("Solar Collector Heat Transfer Energy");
    }
    return result;
  }

  IddObjectType SolarCollectorFlatPlateWater_Impl::iddObjectType() const {
    return SolarCollectorFlatPlateWater::iddObjectType();
  }

  std::vector<ModelObject> SolarCollectorFlatPlateWater_Impl::children() const
  {
    std::vector<ModelObject> result;

    SolarCollectorPerformanceFlatPlate solarCollectorPerformance = this->solarCollectorPerformance();
    result.push_back(solarCollectorPerformance);

    return result;
  }

  unsigned SolarCollectorFlatPlateWater_Impl::inletPort()
  {
    return OS_SolarCollector_FlatPlate_WaterFields::InletNodeName;
  }

  unsigned SolarCollectorFlatPlateWater_Impl::outletPort()
  {
    return OS_SolarCollector_FlatPlate_WaterFields::OutletNodeName;
  }

  bool SolarCollectorFlatPlateWater_Impl::addToNode(Node & node)
  {
    if (boost::optional<PlantLoop> plantLoop = node.plantLoop())
    {
      if (plantLoop->supplyComponent(node.handle()))
      {
        if (StraightComponent_Impl::addToNode(node))
        {
          return true;
        }
      }
    }

    return false;
  }

  SolarCollectorPerformanceFlatPlate SolarCollectorFlatPlateWater_Impl::solarCollectorPerformance() const {
    boost::optional<SolarCollectorPerformanceFlatPlate> value = getObject<ModelObject>().getModelObjectTarget<SolarCollectorPerformanceFlatPlate>(OS_SolarCollector_FlatPlate_WaterFields::SolarCollectorPerformanceName);
    if (!value) {
      // DLM: could default construct one here?
      LOG_AND_THROW(briefDescription() << " does not have an Solar Collector Performance attached.");
    }
    return value.get();
  }

  boost::optional<PlanarSurface> SolarCollectorFlatPlateWater_Impl::surface() const {
    return getObject<ModelObject>().getModelObjectTarget<PlanarSurface>(OS_SolarCollector_FlatPlate_WaterFields::SurfaceName);
  }
 
  boost::optional<double> SolarCollectorFlatPlateWater_Impl::maximumFlowRate() const {
    return getDouble(OS_SolarCollector_FlatPlate_WaterFields::MaximumFlowRate,true);
  }

  bool SolarCollectorFlatPlateWater_Impl::setSolarCollectorPerformance(const SolarCollectorPerformanceFlatPlate& solarCollectorPerformanceFlatPlate) {
    ModelObject clone = solarCollectorPerformanceFlatPlate.clone(this->model());
    return setSolarCollectorPerformanceNoClone(clone.cast<SolarCollectorPerformanceFlatPlate>());
  }
  
  void SolarCollectorFlatPlateWater_Impl::resetSolarCollectorPerformance()
  {
    boost::optional<SolarCollectorPerformanceFlatPlate> oldPerformance = getObject<ModelObject>().getModelObjectTarget<SolarCollectorPerformanceFlatPlate>(OS_SolarCollector_FlatPlate_WaterFields::SolarCollectorPerformanceName);
    if (oldPerformance){
      oldPerformance->remove();
    }
    
    SolarCollectorPerformanceFlatPlate performance(this->model());
    bool ok = setSolarCollectorPerformanceNoClone(performance);
    OS_ASSERT(ok);
  }

  bool SolarCollectorFlatPlateWater_Impl::setSurface(const PlanarSurface& surface) {
    bool result(false);

    // DLM: check for existing solar collectors or photovoltaic generators?
    
    if (surface.optionalCast<Surface>()){
      result = setPointer(OS_SolarCollector_FlatPlate_WaterFields::SurfaceName, surface.handle());
    } else if (surface.optionalCast<ShadingSurface>()){
      result = setPointer(OS_SolarCollector_FlatPlate_WaterFields::SurfaceName, surface.handle());
    }

    return result;
  }

  void SolarCollectorFlatPlateWater_Impl::resetSurface() {
    bool result = setString(OS_SolarCollector_FlatPlate_WaterFields::SurfaceName, "");
    OS_ASSERT(result);
  }

  bool SolarCollectorFlatPlateWater_Impl::setMaximumFlowRate(double maximumFlowRate) {
    return setDouble(OS_SolarCollector_FlatPlate_WaterFields::MaximumFlowRate, maximumFlowRate);
  }

  void SolarCollectorFlatPlateWater_Impl::resetMaximumFlowRate() {
    bool result = setString(OS_SolarCollector_FlatPlate_WaterFields::MaximumFlowRate, "");
    OS_ASSERT(result);
  }
  
  bool SolarCollectorFlatPlateWater_Impl::setSolarCollectorPerformanceNoClone(const SolarCollectorPerformanceFlatPlate& solarCollectorPerformanceFlatPlate) {
    return setPointer(OS_SolarCollector_FlatPlate_WaterFields::SolarCollectorPerformanceName, solarCollectorPerformanceFlatPlate.handle());
  }

} // detail

SolarCollectorFlatPlateWater::SolarCollectorFlatPlateWater(const Model& model)
  : StraightComponent(SolarCollectorFlatPlateWater::iddObjectType(),model)
{
  OS_ASSERT(getImpl<detail::SolarCollectorFlatPlateWater_Impl>());

  SolarCollectorPerformanceFlatPlate performance(model);

  bool ok = true;
  ok = getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->setSolarCollectorPerformanceNoClone(performance);
  OS_ASSERT(ok);

}

IddObjectType SolarCollectorFlatPlateWater::iddObjectType() {
  return IddObjectType(IddObjectType::OS_SolarCollector_FlatPlate_Water);
}

SolarCollectorPerformanceFlatPlate SolarCollectorFlatPlateWater::solarCollectorPerformance() const {
  return getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->solarCollectorPerformance();
}

boost::optional<PlanarSurface> SolarCollectorFlatPlateWater::surface() const {
  return getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->surface();
}

boost::optional<double> SolarCollectorFlatPlateWater::maximumFlowRate() const {
  return getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->maximumFlowRate();
}

bool SolarCollectorFlatPlateWater::setSolarCollectorPerformance(const SolarCollectorPerformanceFlatPlate& solarCollectorPerformanceFlatPlate) {
  return getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->setSolarCollectorPerformance(solarCollectorPerformanceFlatPlate);
}

void SolarCollectorFlatPlateWater::resetSolarCollectorPerformance() {
  getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->resetSolarCollectorPerformance();
}

bool SolarCollectorFlatPlateWater::setSurface(const PlanarSurface& surface) {
  return getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->setSurface(surface);
}

void SolarCollectorFlatPlateWater::resetSurface() {
  return getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->resetSurface();
}

bool SolarCollectorFlatPlateWater::setMaximumFlowRate(double maximumFlowRate) {
  return getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->setMaximumFlowRate(maximumFlowRate);
}

void SolarCollectorFlatPlateWater::resetMaximumFlowRate() {
  getImpl<detail::SolarCollectorFlatPlateWater_Impl>()->resetMaximumFlowRate();
}

/// @cond
SolarCollectorFlatPlateWater::SolarCollectorFlatPlateWater(std::shared_ptr<detail::SolarCollectorFlatPlateWater_Impl> impl)
  : StraightComponent(impl)
{}
/// @endcond

} // model
} // openstudio

