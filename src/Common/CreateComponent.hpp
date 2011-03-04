// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Common_CreateComponent_hpp
#define CF_Common_CreateComponent_hpp

/////////////////////////////////////////////////////////////////////////////////

#include "Common/CBuilder.hpp"
#include "Common/BasicExceptions.hpp"

namespace CF {
namespace Common {

/////////////////////////////////////////////////////////////////////////////////

/// Create a (sub)component of a given abstract type specified type
/// @param provider_name the registry string of the provider of the concrete type
/// @name name to give to the created omponent
template < typename ATYPE >
    typename ATYPE::Ptr create_component_abstract_type ( const std::string& builder_name, const std::string& name )
{
  Component::Ptr factories = Core::instance().root()->get_child_ptr("Factories");
  if ( is_null(factories) ) throw ValueNotFound( FromHere(), "CFactories \'Factories\' not found in " + Core::instance().root()->full_path().string() );

  Component::Ptr factory = factories->get_child_ptr( ATYPE::type_name() );
  if ( is_null(factory) ) throw ValueNotFound( FromHere(), "CFactory \'" + ATYPE::type_name() + "\' not found in " + factories->full_path().string() + ". Probably forgot to load a library." );

  Component::Ptr builder = factory->get_child_ptr( builder_name );
  if ( is_null(builder) ) throw ValueNotFound( FromHere(), "CBuilder \'" + builder_name + "\' not found in factory \'" + ATYPE::type_name() + "\'. Probably forgot to load a library." );

  Component::Ptr comp = builder->as_type<CBuilder>().build ( name );
  if ( is_null(comp) ) throw NotEnoughMemory ( FromHere(), "CBuilder \'" + builder_name + "\' failed to allocate component with name \'" + name + "\'" );

  typename ATYPE::Ptr ccomp = comp->as_ptr<ATYPE>();
  if ( is_null(ccomp) ) throw CastingFailed ( FromHere(), "Pointer created by CBuilder \'" + builder_name + "\' could not be casted to \'" + ATYPE::type_name() + "\' pointer" );

  return ccomp;
}

/////////////////////////////////////////////////////////////////////////////////

} // Common
} // CF

#endif // CF_Common_CreateComponent_hpp
