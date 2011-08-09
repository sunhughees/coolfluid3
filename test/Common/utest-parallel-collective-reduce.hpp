// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

// this file is en-block included into utest-parallel-collective.cpp
// do not include anything here, rather in utest-parallel-collective.cpp

////////////////////////////////////////////////////////////////////////////////

struct PEReduceFixture
{
  /// common setup for each test case
  PEReduceFixture()
  {
    // rank and proc
    nproc=Comm::PE::instance().size();
    irank=Comm::PE::instance().rank();

    // ptr helpers
    sndcnt=0;
    ptr_snddat=new double[2*nproc];
    ptr_rcvdat=new double[2*nproc];
    ptr_tmprcv=new double[2*nproc];
    ptr_sndmap=new int[nproc];
    ptr_rcvmap=new int[nproc];
    ptr_snddat2=new double[4*nproc];
    ptr_rcvdat2=new double[4*nproc];
    ptr_tmprcv2=new double[4*nproc];

    // std::Vector helpers
    vec_snddat.resize(2*nproc);
    vec_rcvdat.resize(2*nproc);
    vec_tmprcv.resize(2*nproc);
    vec_sndmap.resize(nproc);
    vec_rcvmap.resize(nproc);
    vec_snddat2.resize(4*nproc);
    vec_rcvdat2.resize(4*nproc);
    vec_tmprcv2.resize(4*nproc);
  }

  /// common tear-down for each test case
  ~PEReduceFixture()
  {
    delete[] ptr_snddat;
    delete[] ptr_rcvdat;
    delete[] ptr_sndmap;
    delete[] ptr_rcvmap;
    delete[] ptr_tmprcv;
    delete[] ptr_snddat2;
    delete[] ptr_rcvdat2;
    delete[] ptr_tmprcv2;
  }

  /// number of processes
  int nproc;
  /// rank of process
  int irank;

  /// data for raw pointers
  int     sndcnt;
  double* ptr_snddat;
  double* ptr_rcvdat;
  double* ptr_tmprcv;
  int*    ptr_sndmap;
  int*    ptr_rcvmap;
  double* ptr_snddat2;
  double* ptr_rcvdat2;
  double* ptr_tmprcv2;

  /// data for std::vectors
  std::vector<double> vec_snddat;
  std::vector<double> vec_rcvdat;
  std::vector<double> vec_tmprcv;
  std::vector<int>    vec_sndmap;
  std::vector<int>    vec_rcvmap;
  std::vector<double> vec_snddat2;
  std::vector<double> vec_rcvdat2;
  std::vector<double> vec_tmprcv2;

  /// helper function for constant size data - setting up input and verification data
  void setup_data_constant()
  {
    int i,j,k;
    for (i=0; i<2*nproc; i++)
    {
      ptr_snddat[i]=(irank+1)*10000+(i+1);
      ptr_rcvdat[i]=0.;
      for (k=0; k<nproc; k++) ptr_rcvdat[i]+=(k+1)*10000+(i+1);
    }
    for (i=0; i<2*nproc; i++) { ptr_snddat2[2*i+0]=ptr_snddat[i]; ptr_snddat2[2*i+1]=ptr_snddat[i]+1; }
    for (i=0; i<2*nproc; i++) { ptr_rcvdat2[2*i+0]=ptr_rcvdat[i]; ptr_rcvdat2[2*i+1]=ptr_rcvdat[i]+nproc; }
    sndcnt=2*nproc;
    vec_snddat.assign(ptr_snddat,ptr_snddat+2*nproc);
    vec_rcvdat.assign(ptr_rcvdat,ptr_rcvdat+2*nproc);
    vec_snddat2.assign(ptr_snddat2,ptr_snddat2+4*nproc);
    vec_rcvdat2.assign(ptr_rcvdat2,ptr_rcvdat2+4*nproc);
  }

  /// helper function for variable size data - setting up input and verification data
  void setup_data_variable()
  {
    int i,j,k;
    for (i=0; i<2*nproc; i++) {
      ptr_snddat[i]=-1.;
      ptr_rcvdat[i]=0.;
    }
    for (i=0; i<nproc; i++)
    {
      ptr_snddat[2*i]=(irank+1)*10000+(i+1);
      ptr_sndmap[i]=2*i; // taking only every second
      ptr_rcvmap[i]=2*(nproc-1-i); // inverts order and puts to every second
      for (k=0; k<nproc; k++) ptr_rcvdat[2*(nproc-1-i)]+=(k+1)*10000+(i+1);
    }
    for (i=0; i<2*nproc; i++) { ptr_snddat2[2*i+0]=ptr_snddat[i]; ptr_snddat2[2*i+1]=ptr_snddat[i]+1; }
    for (i=0; i<2*nproc; i++) { ptr_rcvdat2[2*i+0]=ptr_rcvdat[i]; ptr_rcvdat2[2*i+1]=ptr_rcvdat[i]+(double)nproc; }
    sndcnt=nproc;
    vec_snddat.assign(ptr_snddat,ptr_snddat+2*nproc);
    vec_rcvdat.assign(ptr_rcvdat,ptr_rcvdat+2*nproc);
    vec_sndmap.assign(ptr_sndmap,ptr_sndmap+nproc);
    vec_rcvmap.assign(ptr_rcvmap,ptr_rcvmap+nproc);
    vec_snddat2.assign(ptr_snddat2,ptr_snddat2+4*nproc);
    vec_rcvdat2.assign(ptr_rcvdat2,ptr_rcvdat2+4*nproc);
  }

  /// test class with operator + to test if operations and reduce can work with it
  class optest {
    public:
      /// simple data
      int ival;
      double dval;
      /// giving values for i and d
      void init(){
        ival=Comm::PE::instance().rank()+1;
        dval=(double)Comm::PE::instance().rank()+10.;
      }
      /// operator +
      optest operator +(const optest& b) const { optest t; t.ival=ival+b.ival; t.dval=dval+b.dval; return t; };
      /// function to test result
      bool test()
      {
        int i;
        int nproc=Comm::PE::instance().size();
        int itest=0;
        for(i=0; i<nproc; i++) itest+=i+1;
        double dtest=0;
        for(i=0; i<nproc; i++) dtest+=(double)i+10.;
        return ((ival==itest)&&(dval==dtest));
      }
  };

};

////////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE( PEReduceSuite, PEReduceFixture )

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( reduce )
{
  PEProcessSortedExecute(-1,CFinfo << "Testing reduce " << irank << "/" << nproc << CFendl; );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( reduce_most_common_ops )
{
  int i,r;
  int ival,itest,iresult;
  double dval,dtest,dresult;
  ival=irank+1;
  dval=(double)irank+1.;

  // testing plus
  itest=0;
  dtest=0.;
  for(i=0; i<nproc; i++) { itest+=i+1; dtest+=(double)i+1.; }
  iresult=-1;
  dresult=-1.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), &ival, 1, &iresult, r);
  BOOST_CHECK_EQUAL( iresult, itest );
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), &dval, 1, &dresult, r);
  BOOST_CHECK_EQUAL( dresult, dtest );

  // testing multiplies
  itest=1;
  dtest=1.;
  for(i=0; i<nproc; i++) { itest*=i+1; dtest*=(double)i+1.; }
  iresult=-1;
  dresult=-1.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::multiplies(), &ival, 1, &iresult, r);
  BOOST_CHECK_EQUAL( iresult, itest );
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::multiplies(), &dval, 1, &dresult, r);
  BOOST_CHECK_EQUAL( dresult, dtest );

  // testing max
  itest=nproc;
  dtest=(double)nproc;
  iresult=-1;
  dresult=-1.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::max(), &ival, 1, &iresult, r);
  BOOST_CHECK_EQUAL( iresult, itest );
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::max(), &dval, 1, &dresult, r);
  BOOST_CHECK_EQUAL( dresult, dtest );

  // testing min
  itest=1;
  dtest=1.;
  iresult=-1;
  dresult=-1.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::min(), &ival, 1, &iresult, r);
  BOOST_CHECK_EQUAL( iresult, itest );
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::min(), &dval, 1, &dresult, r);
  BOOST_CHECK_EQUAL( dresult, dtest );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( reduce_operator_of_class )
{
  int r;
  optest in[3],out[3];
  in[0].init();
  in[1].init();
  in[2].init();
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), in, 3, out, r);
  BOOST_CHECK_EQUAL( out[0].test() , true );
  BOOST_CHECK_EQUAL( out[1].test() , true );
  BOOST_CHECK_EQUAL( out[2].test() , true );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( reduce_ptr_constant )
{
  int i,r;

  setup_data_constant();

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  for(r=0; r<nproc; r++) {
    if (r==irank) {
      ptr_tmprcv=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat, sndcnt, (double*)0, r);
    } else {
      double* dummy=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat, sndcnt, (double*)0, r);
      BOOST_CHECK_EQUAL( dummy , (double*)0 );
    }
  }
  for (i=0; i<2*nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );

  for (i=0; i<2*nproc; i++) ptr_tmprcv[i]=0.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), ptr_snddat, sndcnt, ptr_tmprcv, r);
  for (i=0; i<2*nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );

  for(r=0; r<nproc; r++) {
    for (i=0; i<2*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
    Comm::PE::instance().reduce(MPI::plus(), ptr_tmprcv, sndcnt, ptr_tmprcv, r);
    if (irank==r) {
      for (i=0; i<2*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );
    } else {
      for (i=0; i<2*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_snddat[i] );
    }
  }

  delete[] ptr_tmprcv2;
  ptr_tmprcv2=0;
  for(r=0; r<nproc; r++) {
    if (r==irank) {
      ptr_tmprcv2=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat2, sndcnt, (double*)0, r, 2);
    } else {
      double* dummy=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat2, sndcnt, (double*)0, r, 2);
      BOOST_CHECK_EQUAL( dummy , (double*)0 );
    }
  }
  for (i=0; i<4*nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[i] , ptr_rcvdat2[i] );

  for (i=0; i<4*nproc; i++) ptr_tmprcv2[i]=0.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), ptr_snddat2, sndcnt, ptr_tmprcv2, r, 2);
  for (i=0; i<4*nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[i] , ptr_rcvdat2[i] );

  for(r=0; r<nproc; r++) {
    for (i=0; i<4*nproc; i++) ptr_tmprcv2[i]=ptr_snddat2[i];
    Comm::PE::instance().reduce(MPI::plus(), ptr_tmprcv2, sndcnt, ptr_tmprcv2, r, 2);
    if (irank==r) {
      for (i=0; i<4*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv2[i] , ptr_rcvdat2[i] );
    } else {
      for (i=0; i<4*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv2[i] , ptr_snddat2[i] );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( reduce_vector_constant )
{
  int i,r;

  setup_data_constant();

  vec_tmprcv.resize(0);
  vec_tmprcv.reserve(0);
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat, vec_tmprcv, r);
  for (i=0; i<2*nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv[i] , vec_rcvdat[i] );
  BOOST_CHECK_EQUAL( (int)vec_tmprcv.size() , sndcnt );

  vec_tmprcv.assign(2*nproc,0.);
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat, vec_tmprcv, r);
  for (i=0; i<2*nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv[i] , vec_rcvdat[i] );

  for(r=0; r<nproc; r++) {
    vec_tmprcv=vec_snddat;
    Comm::PE::instance().reduce(MPI::plus(), vec_tmprcv, vec_tmprcv, r);
    if (irank==r) {
      for (i=0; i<2*nproc; i++)  BOOST_CHECK_EQUAL( vec_tmprcv[i] , vec_rcvdat[i] );
    } else {
      for (i=0; i<2*nproc; i++)  BOOST_CHECK_EQUAL( vec_tmprcv[i] , vec_snddat[i] );
    }
  }

  vec_tmprcv2.resize(0);
  vec_tmprcv2.reserve(0);
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat2, vec_tmprcv2, r, 2);
  for (i=0; i<4*nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[i] , vec_rcvdat2[i] );
  BOOST_CHECK_EQUAL( (int)vec_tmprcv2.size() , 2*sndcnt );

  vec_tmprcv2.assign(4*nproc,0.);
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat2, vec_tmprcv2, r, 2);
  for (i=0; i<4*nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[i] , vec_rcvdat2[i] );

  for(r=0; r<nproc; r++) {
    vec_tmprcv2=vec_snddat2;
    Comm::PE::instance().reduce(MPI::plus(), vec_tmprcv2, vec_tmprcv2, r, 2);
    if (irank==r) {
      for (i=0; i<4*nproc; i++)  BOOST_CHECK_EQUAL( vec_tmprcv2[i] , vec_rcvdat2[i] );
    } else {
      for (i=0; i<4*nproc; i++)  BOOST_CHECK_EQUAL( vec_tmprcv2[i] , vec_snddat2[i] );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( reduce_ptr_variable )
{
  int i,r;

  setup_data_variable();

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  for(r=0; r<nproc; r++) {
    if (r==irank) {
      ptr_tmprcv=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat, sndcnt, ptr_sndmap, (double*)0, ptr_rcvmap, r);
    } else {
      double* dummy=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat, sndcnt, ptr_sndmap, (double*)0, ptr_rcvmap, r);
      BOOST_CHECK_EQUAL( dummy , (double*)0 );
    }
  }
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv[2*i] , ptr_rcvdat[2*i] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=new double[2*nproc];
  for (i=0; i<2*nproc; i++) ptr_tmprcv[i]=0.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), ptr_snddat, sndcnt, ptr_sndmap, ptr_tmprcv, ptr_rcvmap, r);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv[2*i] , ptr_rcvdat[2*i] );

  for(r=0; r<nproc; r++) {
    for (i=0; i<2*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
    Comm::PE::instance().reduce(MPI::plus(), ptr_tmprcv, sndcnt, ptr_sndmap, ptr_tmprcv, ptr_rcvmap, r);
    if (irank==r) {
      for (i=0; i<nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[2*i] , ptr_rcvdat[2*i] );
    } else {
      for (i=0; i<nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[2*i] , ptr_snddat[2*i] );
    }
  }

  delete[] ptr_tmprcv2;
  ptr_tmprcv2=0;
  for(r=0; r<nproc; r++) {
    if (r==irank) {
      ptr_tmprcv2=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat2, sndcnt, ptr_sndmap, (double*)0, ptr_rcvmap, r, 2);
    } else {
      double* dummy=Comm::PE::instance().reduce(MPI::plus(), ptr_snddat2, sndcnt, ptr_sndmap, (double*)0, ptr_rcvmap, r, 2);
      BOOST_CHECK_EQUAL( dummy , (double*)0 );
    }
  }
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+0] , ptr_rcvdat2[4*i+0] );
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+1] , ptr_rcvdat2[4*i+1] );

  delete[] ptr_tmprcv2;
  ptr_tmprcv2=new double[4*nproc];
  for (i=0; i<4*nproc; i++) ptr_tmprcv2[i]=0.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), ptr_snddat2, sndcnt, ptr_sndmap, ptr_tmprcv2, ptr_rcvmap, r, 2);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+0] , ptr_rcvdat2[4*i+0] );
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+1] , ptr_rcvdat2[4*i+1] );

  for(r=0; r<nproc; r++) {
    for (i=0; i<4*nproc; i++) ptr_tmprcv2[i]=ptr_snddat2[i];
    Comm::PE::instance().reduce(MPI::plus(), ptr_tmprcv2, sndcnt, ptr_sndmap, ptr_tmprcv2, ptr_rcvmap, r, 2);
    if (irank==r) {
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+0] , ptr_rcvdat2[4*i+0] );
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+1] , ptr_rcvdat2[4*i+1] );
    } else {
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+0] , ptr_snddat2[4*i+0] );
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv2[4*i+1] , ptr_snddat2[4*i+1] );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( reduce_vector_variable )
{
  int i,r;

  setup_data_variable();

  vec_tmprcv.resize(0);
  vec_tmprcv.reserve(0);
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat, vec_sndmap, vec_tmprcv, vec_rcvmap, r);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv[2*i] , vec_rcvdat[2*i] );

  vec_tmprcv.resize(2*nproc);
  vec_tmprcv.reserve(2*nproc);
  for (i=0; i<2*nproc; i++) vec_tmprcv[i]=0.;
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat, vec_sndmap, vec_tmprcv, vec_rcvmap, r);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv[2*i] , vec_rcvdat[2*i] );

  for(r=0; r<nproc; r++) {
    vec_tmprcv=vec_snddat;
    Comm::PE::instance().reduce(MPI::plus(), vec_tmprcv, vec_sndmap, vec_tmprcv, vec_rcvmap, r);
    if (irank==r) {
      for (i=0; i<nproc; i++)  BOOST_CHECK_EQUAL( vec_tmprcv[2*i] , vec_rcvdat[2*i] );
    } else {
      for (i=0; i<nproc; i++)  BOOST_CHECK_EQUAL( vec_tmprcv[2*i] , vec_snddat[2*i] );
    }
  }

  vec_tmprcv2.resize(0);
  vec_tmprcv2.reserve(0);
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat2, vec_sndmap, vec_tmprcv2, vec_rcvmap, r, 2);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+0] , vec_rcvdat2[4*i+0] );
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+1] , vec_rcvdat2[4*i+1] );

  vec_tmprcv2.resize(4*nproc);
  vec_tmprcv2.reserve(4*nproc);
  vec_tmprcv2.assign(4*nproc,0.);
  for(r=0; r<nproc; r++) Comm::PE::instance().reduce(MPI::plus(), vec_snddat2, vec_sndmap, vec_tmprcv2, vec_rcvmap, r, 2);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+0] , vec_rcvdat2[4*i+0] );
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+1] , vec_rcvdat2[4*i+1] );

  for(r=0; r<nproc; r++) {
    vec_tmprcv2=vec_snddat2;
    Comm::PE::instance().reduce(MPI::plus(), vec_tmprcv2, vec_sndmap, vec_tmprcv2, vec_rcvmap, r, 2);
    if (irank==r) {
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+0] , vec_rcvdat2[4*i+0] );
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+1] , vec_rcvdat2[4*i+1] );
    } else {
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+0] , vec_snddat2[4*i+0] );
      for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv2[4*i+1] , vec_snddat2[4*i+1] );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////
