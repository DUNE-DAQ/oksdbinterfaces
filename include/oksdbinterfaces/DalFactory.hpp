#ifndef DAL_FACTORY_H
#define DAL_FACTORY_H

#include <map>
#include <mutex>
#include <string>

#include "ers/ers.hpp"

#include "oksdbinterfaces/set.hpp"
#include "oksdbinterfaces/DalFactoryFunctions.hpp"

#include "logging/Logging.hpp"

namespace dunedaq {
namespace oksdbinterfaces {

class DalFactory
{

public:

  /** return the singleton */
  static DalFactory &
  instance();

  /** register DAL object creator by class name*/
  template<class T>
    void
    register_dal_class(const std::string & name, const std::set<std::string>& algorithms)
    {
      std::lock_guard<std::mutex> scoped_lock(m_class_mutex);

      TLOG_DEBUG(1) << "register class " << name;

      if (m_classes.emplace(name, DalFactoryFunctions(boost::compute::identity<T>(), algorithms)).second == false)
        {
          TLOG_DEBUG(0) << "class " << name << " was already registered";
        }
    }

  const std::string&
  get_known_class_name_ref(const std::string& name)
  {
    std::lock_guard<std::mutex> scoped_lock(m_known_class_mutex);
    return *m_known_classes.emplace(name).first;
  }


  /**
   * \brief Get DAL object from oksdbinterfaces object
   *
   * \param db                    configuration database object
   * \param obj                   oksdbinterfaces object
   * \param uid                   uid for generated objects
   * \param upcast_unregistered   if true and and native DAL class of oksdbinterfaces object is not registered, search an appropriate base class within superclasses hierarchy
   * \return                      the DAL object
   *
   * \throw                       oksdbinterfaces::Generic exception if class of object is not registered
   */

  DalObject *
  get(Configuration& db, ConfigObject& obj, const std::string& uid, bool upcast_unregistered) const;

  DalObject *
  get(Configuration& db, ConfigObject& obj, const std::string& uid, const std::string& class_name) const;


  /**
   * \brief Get factory function
   *
   * \param db                    configuration database object
   * \param                       name of OKS class
   * \param upcast_unregistered   if true and native DAL class of oksdbinterfaces object is not registered, search an appropriate base class within superclasses hierarchy
   * \return                      the factory functions for given class
   */

  const DalFactoryFunctions&
  functions(const Configuration& db, const std::string& name, bool upcast_unregistered) const;


  const std::string&
  class4algo(Configuration& db, const std::string& name, const std::string& algorithm) const;

  /**
   * \brief Get factory function
   *
   * \param                       name of OKS class
   * \return                      the factory functions for given class
   */

  const DalFactoryFunctions&
  functions(const std::string& name) const;


private:

  std::mutex m_class_mutex;
  std::map<std::string, DalFactoryFunctions> m_classes;

  std::mutex m_known_class_mutex;
  oksdbinterfaces::set m_known_classes;
};

} // namespace oksdbinterfaces
} // namespace dunedaq

#endif
