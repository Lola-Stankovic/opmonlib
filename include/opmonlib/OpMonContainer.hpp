/**
 * @file OpMonContainer.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef OPMONLIB_INCLUDE_OPMONLIB_OPMONCONTAINER_HPP_
#define OPMONLIB_INCLUDE_OPMONLIB_OPMONCONTAINER_HPP_

#include <nlohmann/json.hpp>

#include <iostream>
#include <ctime>
#include <chrono>
#include <list>

#include <google/protobuf/message.h>
#include "opmonlib/opmon_entry.pb.h"

#include <google/protobuf/util/time_util.h>

namespace dunedaq::opmonlib {

class OpMonContainer
{
  std::list<dunedaq::opmon::OpMonEntry> m_entries;
  
public:

  const auto & entries() const { return m_entries; }

  void add(::google::protobuf::Message && m, std::string id="")
  {
    auto timestamp = google::protobuf::util::TimeUtil::GetCurrentTime();
    
    dunedaq::opmon::OpMonEntry entry;
    *entry.mutable_time() = timestamp;
    entry.set_opmon_id( id );
    entry.set_measurement( m.GetTypeName() );
    
    const auto * descriptor_p = m.GetDescriptor();
    const auto & des = *descriptor_p;

    const auto * reflection_p = m.GetReflection();
    const auto & ref = *reflection_p;

    using namespace google::protobuf;
    
    auto count = des.field_count();
    for ( int i = 0; i < count; ++i ) {
      const auto * field_p = des.field(i);
      if ( field_p -> is_repeated() ) continue;
      auto name = field_p -> name();
      auto type = field_p -> cpp_type();
      dunedaq::opmon::OpMonValue value;
      bool success = true;
      switch (type) {
      case FieldDescriptor::CppType::CPPTYPE_INT32:
	value.set_int4_value( ref.GetInt32(m, field_p) );
	break;
      default:
	success = false;
	break;
      }
      if ( success ) 
	(*entry.mutable_data())[name] = value;
    }

    if ( entry.data().size() > 0 ) 
      m_entries.push_back( entry );
  }

};

} // namespace dunedaq::opmonlib

#endif // OPMONLIB_INCLUDE_OPMONLIB_OPMONCONTAINER_HPP_
