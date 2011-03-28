// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <iostream>

#include <QApplication>

#include "Common/CF.hpp"
#include "Common/Core.hpp"
#include "Common/CEnv.hpp"
#include "Common/NetworkInfo.hpp"
#include "Common/Exception.hpp"

#include "GUI/Core/ThreadManager.hpp"

#include "GUI/Graphics/MainWindow.hpp"
#include "GUI/Graphics/JournalBrowserDialog.hpp"

using namespace CF::Common;
using namespace CF::GUI::Core;
using namespace CF::GUI::Graphics;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  int returnValue;

  JournalBrowserBuilder::instance();

  ThreadManager::instance().tree();

  // initiate the core environment
  Core::instance().environment()->configure_property("RegistSignalHandlers", false);
  Core::instance().initiate(argc, argv);

  CF::AssertionManager::instance().AssertionThrows = true;
  CF::AssertionManager::instance().AssertionDumps = true;

  // tell CF core that the client is running
  Core::instance().network_info()->start_client();

  try
  {
   MainWindow window;
   window.showMaximized();
   returnValue = app.exec();
  }
  catch(Exception e)
  {
    std::cerr << "Application stopped on uncaught exception:" << std::endl;
    std::cerr << e.what() << std::endl;
    returnValue = -1;
  }

  // tell CF core that the client is about to exit
  Core::instance().network_info()->stop_client();


  return returnValue;
}
