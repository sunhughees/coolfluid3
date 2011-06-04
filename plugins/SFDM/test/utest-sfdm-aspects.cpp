// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for CF::SFDM"

#include <boost/test/unit_test.hpp>


#include "Common/Log.hpp"
#include "Common/Core.hpp"
#include "Common/CRoot.hpp"
#include "Common/FindComponents.hpp"
#include "Mesh/CMesh.hpp"
#include "Mesh/CRegion.hpp"
#include "Mesh/CField.hpp"
#include "Mesh/CSimpleMeshGenerator.hpp"
#include "Mesh/CEntities.hpp"
#include "Mesh/ElementType.hpp"
#include "Mesh/CMeshWriter.hpp"
#include "Mesh/Actions/CInitFieldFunction.hpp"
#include "Mesh/Actions/CreateSpaceP0.hpp"
#include "SFDM/CreateSpace.hpp"
#include "SFDM/ShapeFunction.hpp"
#include "SFDM/Reconstruct.hpp"

using namespace CF;
using namespace CF::Mesh;
using namespace CF::Common;
using namespace CF::SFDM;

//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( SFDM_Suite )

//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( test_SF_lines )
{
  typedef SFDM::ShapeFunction SFD_SF;
  CRoot& root = Core::instance().root();
  SFD_SF& sol_line_p0 = root.create_component("sol_line_p0","CF.SFDM.SF.LineSolutionP0").as_type<SFD_SF>();
  SFD_SF& sol_line_p1 = root.create_component("sol_line_p1","CF.SFDM.SF.LineSolutionP1").as_type<SFD_SF>();
  SFD_SF& sol_line_p2 = root.create_component("sol_line_p2","CF.SFDM.SF.LineSolutionP2").as_type<SFD_SF>();

  SFD_SF& flx_line_p1 = root.create_component("flx_line_p1","CF.SFDM.SF.LineFluxP1").as_type<SFD_SF>();
  SFD_SF& flx_line_p2 = root.create_component("flx_line_p2","CF.SFDM.SF.LineFluxP2").as_type<SFD_SF>();
  SFD_SF& flx_line_p3 = root.create_component("flx_line_p3","CF.SFDM.SF.LineFluxP3").as_type<SFD_SF>();

  const Uint line0 = 0;

  SFD_SF::Points     all_pts   = flx_line_p2.points();
  SFD_SF::Lines      ksi_pts   = all_pts[KSI];
  SFD_SF::LinePoints line0_pts = ksi_pts[line0];

  BOOST_CHECK_EQUAL( all_pts.num_elements()   , 3u ); // 3 points total

  BOOST_CHECK_EQUAL( all_pts.size()   , 1u ); // 1 orientation KSI
  BOOST_CHECK_EQUAL( ksi_pts.size()   , 1u ); // 1 line in this orientation
  BOOST_CHECK_EQUAL( line0_pts.size() , 3u ); // 3 points in this line

  BOOST_CHECK_EQUAL( flx_line_p1.points()[KSI][line0][0] , 0u );
  BOOST_CHECK_EQUAL( flx_line_p1.points()[KSI][line0][1] , 1u );

  BOOST_CHECK_EQUAL( flx_line_p1.face_points()[KSI][line0][LEFT] , 0u );
  BOOST_CHECK_EQUAL( flx_line_p1.face_points()[KSI][line0][RIGHT], 1u );

  BOOST_CHECK_EQUAL( flx_line_p2.points()[KSI][line0][0] , 0u );
  BOOST_CHECK_EQUAL( flx_line_p2.points()[KSI][line0][1] , 1u );
  BOOST_CHECK_EQUAL( flx_line_p2.points()[KSI][line0][2] , 2u );

  BOOST_CHECK_EQUAL( flx_line_p2.face_points()[KSI][line0][LEFT] , 0u );
  BOOST_CHECK_EQUAL( flx_line_p2.face_points()[KSI][line0][RIGHT], 2u );

  BOOST_CHECK_EQUAL( flx_line_p3.points()[KSI][line0][0] , 0u );
  BOOST_CHECK_EQUAL( flx_line_p3.points()[KSI][line0][1] , 1u );
  BOOST_CHECK_EQUAL( flx_line_p3.points()[KSI][line0][2] , 2u );
  BOOST_CHECK_EQUAL( flx_line_p3.points()[KSI][line0][3] , 3u );

  BOOST_CHECK_EQUAL( flx_line_p3.face_points()[KSI][line0][LEFT] , 0u);
  BOOST_CHECK_EQUAL( flx_line_p3.face_points()[KSI][line0][RIGHT], 3u);

}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( test_Reconstruction_lines )
{
  CRoot& root = Common::Core::instance().root();
  Reconstruct& reconstruct = root.create_component("reconstruct","CF.SFDM.Reconstruct").as_type<Reconstruct>();
  std::vector<std::string> from_to(2);
  from_to[0] = "CF.SFDM.SF.LineSolutionP1";
  from_to[1] = "CF.SFDM.SF.LineFluxP2";
  reconstruct.configure_property("from_to",from_to);

  RealVector solution(2); solution << 0. , 4.;      // in cell <-1,1>
  RealVector rec_sol(3);  rec_sol  << 0. , 2. , 4.;
  RealVector rec_grad(3); rec_grad << 2. , 2. , 2.;
  BOOST_CHECK_EQUAL ( reconstruct.value(solution) ,        rec_sol  ) ;
  BOOST_CHECK_EQUAL ( reconstruct.gradient(solution,KSI) , rec_grad ) ;
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( test_SF_quads )
{
  typedef SFDM::ShapeFunction SFD_SF;
  CRoot& root = Core::instance().root();
  SFD_SF& sol_quad_p0 = root.create_component("sol_quad_p0","CF.SFDM.SF.QuadSolutionP0").as_type<SFD_SF>();
  SFD_SF& flx_quad_p1 = root.create_component("flx_quad_p1","CF.SFDM.SF.QuadFluxP1").as_type<SFD_SF>();


  BOOST_CHECK_EQUAL( sol_quad_p0.nb_nodes() , 1u );
  BOOST_CHECK_EQUAL( sol_quad_p0.line().derived_type_name() , std::string("CF.SFDM.SF.LineSolutionP0") );

  BOOST_CHECK_EQUAL( flx_quad_p1.nb_nodes() , 4u );
  BOOST_CHECK_EQUAL( flx_quad_p1.line().derived_type_name() , std::string("CF.SFDM.SF.LineFluxP1") );

  const Uint line0 = 0;

  SFD_SF::Points     all_pts   = flx_quad_p1.points();
  SFD_SF::Lines      ksi_pts   = all_pts[KSI];
  SFD_SF::Lines      eta_pts   = all_pts[ETA];
  SFD_SF::LinePoints line0_pts = ksi_pts[line0];

  BOOST_CHECK_EQUAL( all_pts.num_elements()   , 4u ); // 4 points total
  BOOST_CHECK_EQUAL( all_pts.size()   , 2u ); // 2 orientations KSI,ETA
  BOOST_CHECK_EQUAL( ksi_pts.size()   , 1u ); // 1 line in this orientation
  BOOST_CHECK_EQUAL( eta_pts.size()   , 1u ); // 1 line in this orientation
  BOOST_CHECK_EQUAL( line0_pts.size() , 2u ); // 1 points in this line

  BOOST_CHECK_EQUAL( flx_quad_p1.points()[KSI][line0][0] , 0u );
  BOOST_CHECK_EQUAL( flx_quad_p1.points()[KSI][line0][1] , 1u );

  BOOST_CHECK_EQUAL( flx_quad_p1.face_points()[KSI][line0][LEFT] , 0u );
  BOOST_CHECK_EQUAL( flx_quad_p1.face_points()[KSI][line0][RIGHT], 1u );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( test_Reconstruction_quads )
{
  CRoot& root = Common::Core::instance().root();
  Reconstruct& reconstruct = root.create_component("reconstruct_quads","CF.SFDM.Reconstruct").as_type<Reconstruct>();
  std::vector<std::string> from_to(2);
  from_to[0] = "CF.SFDM.SF.QuadSolutionP0";
  from_to[1] = "CF.SFDM.SF.QuadFluxP1";
  reconstruct.configure_property("from_to",from_to);

  RealVector solution(1); solution << 2. ;      // in cell <-1,1><-1,1>
  RealVector rec_sol(4);  rec_sol  << 2. , 2. , 2. , 2.;
  RealVector rec_grad(4); rec_grad << 0. , 0. , 0. , 0.;
  BOOST_CHECK_EQUAL ( reconstruct.value(solution) ,        rec_sol  ) ;
  BOOST_CHECK_EQUAL ( reconstruct.gradient(solution,KSI) , rec_grad ) ;
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( test_fields_lines )
{
  /// Create a mesh consisting of a line with length 1. and 20 divisions
  CMesh::Ptr mesh = Common::Core::instance().root().create_component_ptr<CMesh>("mesh");
  CSimpleMeshGenerator::create_line(*mesh, 1., 20);


  /// Create a "space" for SFDM solution of order P2, and for flux a space of order P3
  SFDM::CreateSpace::Ptr space_creator = allocate_component<SFDM::CreateSpace>("space_creator");
  space_creator->configure_property("P",2u);
  space_creator->transform(*mesh);

  /// create the solution field with space "solution"
  CField& solution = mesh->create_field("solution_field",CField::Basis::CELL_BASED,"solution","var[scalar]");

  /// create the flux field (just for show) with space "flux"
  CField& flux = mesh->create_field("flux_field",CField::Basis::CELL_BASED,"flux","var[scalar]");

  /// Output some stuff
  //CFinfo << mesh->tree() << CFendl;
  CFinfo << "elements = " << mesh->topology().recursive_elements_count() << CFendl;
  CFinfo << "solution_fieldsize = " << solution.size() << CFendl;

  /// Initialize solution field with the function sin(2*pi*x)
  Actions::CInitFieldFunction::Ptr init_field = Common::Core::instance().root().create_component_ptr<Actions::CInitFieldFunction>("init_field");
  init_field->configure_property("functions",std::vector<std::string>(1,"sin(2*pi*x)"));
  init_field->configure_property("field",solution.uri());
  init_field->transform(*mesh);

  //CFinfo << "initialized solution field with data:\n" << solution.data() << CFendl;

  /// write gmsh file. note that gmsh gets really confused because of the multistate view
  CMeshWriter::Ptr gmsh_writer = build_component_abstract_type<CMeshWriter>("CF.Mesh.Gmsh.CWriter","meshwriter");
  gmsh_writer->set_fields(std::vector<CField::Ptr>(1,solution.as_ptr<CField>()));
  gmsh_writer->write_from_to(*mesh,URI("line.msh"));

  CFinfo << "Mesh \"line.msh\" written" << CFendl;

}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( test_fields_quads )
{
  /// Create a mesh consisting of a line with length 1. and 20 divisions
  CMesh::Ptr mesh = Common::Core::instance().root().create_component_ptr<CMesh>("rectangle");
  CSimpleMeshGenerator::create_rectangle(*mesh, 1., 1., 20, 20);


  /// Create a "space" for SFDM solution of order P2, and for flux a space of order P3
  SFDM::CreateSpace::Ptr space_creator = allocate_component<SFDM::CreateSpace>("space_creator");
  space_creator->configure_property("P",0u);
  space_creator->transform(*mesh);

  /// create the solution field with space "solution"
  CField& solution = mesh->create_field("solution_field",CField::Basis::CELL_BASED,"solution","var[scalar]");

  /// create the flux field (just for show) with space "flux"
  CField& flux = mesh->create_field("flux_field",CField::Basis::CELL_BASED,"flux","var[scalar]");

  /// Output some stuff
  //CFinfo << mesh->tree() << CFendl;
  CFinfo << "elements = " << mesh->topology().recursive_elements_count() << CFendl;
  CFinfo << "solution_fieldsize = " << solution.size() << CFendl;

  /// Initialize solution field with the function exp( -( (x-mu)^2+(y-mu)^2 )/(2*sigma^2) )
  Actions::CInitFieldFunction::Ptr init_field = Common::Core::instance().root().create_component_ptr<Actions::CInitFieldFunction>("init_field");
  std::string gaussian="sigma:="+to_str(0.1)+"; mu:="+to_str(0.5)+"; exp( -( (x-mu)^2+(y-mu)^2 )/(2*sigma^2) )";
  init_field->configure_property("functions",std::vector<std::string>(1,gaussian));
  init_field->configure_property("field",solution.uri());
  init_field->transform(*mesh);

  //CFinfo << "initialized solution field with data:\n" << solution.data() << CFendl;

  /// write gmsh file. note that gmsh gets really confused because of the multistate view
  CMeshWriter::Ptr gmsh_writer = build_component_abstract_type<CMeshWriter>("CF.Mesh.Gmsh.CWriter","meshwriter");
  gmsh_writer->set_fields(std::vector<CField::Ptr>(1,solution.as_ptr<CField>()));
  gmsh_writer->write_from_to(*mesh,URI("rectangle.msh"));

  CFinfo << "Mesh \"rectangle.msh\" written" << CFendl;

}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////

