###  feelpp.directive.cmake; coding: utf-8 ---

#  Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
#       Date: 2013-02-04
#
#  Copyright (C) 2013-2015 Feel++ Consortium
#
# Distributed under the GPL(GNU Public License):
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#

set(FEELPP_SCHEDULER "None")
if ( FEELPP_ENABLE_SCHED_LOADLEVELER )
   set( FEELPP_SCHEDULER "LoadLeveler" )
elseif ( FEELPP_ENABLE_SCHED_SLURM )
     set( FEELPP_SCHEDULER "Slurm"  )
elseif ( FEELPP_ENABLE_SCHED_OAR )
     set( FEELPP_SCHEDULER "oar"  )
elseif ( FEELPP_ENABLE_SCHED_CCC )
     set( FEELPP_SCHEDULER "CCC"  )
endif()

################################################################################
# Display a post-configuration installation directives list
################################################################################

MESSAGE(STATUS "================================================================================")
MESSAGE(STATUS "     FEELPP_VERSION_MAJOR : ${FEELPP_VERSION_MAJOR}")
MESSAGE(STATUS "     FEELPP_VERSION_MINOR : ${FEELPP_VERSION_MINOR}")
MESSAGE(STATUS "     FEELPP_VERSION_MICRO : ${FEELPP_VERSION_MICRO}")
MESSAGE(STATUS "FEELPP_VERSION_PRERELEASE : ${FEELPP_VERSION_PRERELEASE}")
MESSAGE(STATUS "  FEELPP_VERSION_METADATA : ${FEELPP_VERSION_METADATA}")
MESSAGE(STATUS "    FEELPP_VERSION_STRING : ${FEELPP_VERSION_STRING}")
MESSAGE(STATUS "           FEELPP_REVISON : ${FEELPP_REVISION}")
MESSAGE(STATUS "           FEELPP_BUILDID : ${FEELPP_BUILDID}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Feel++ Modules :")
MESSAGE(STATUS "     QuickStart: ${FEELPP_ENABLE_QUICKSTART}")
MESSAGE(STATUS "  Documentation: ${FEELPP_ENABLE_DOCUMENTATION}")
MESSAGE(STATUS "        Doxygen: ${FEELPP_ENABLE_DOXYGEN}")
MESSAGE(STATUS "      Testsuite: ${FEELPP_ENABLE_TESTS}")
MESSAGE(STATUS "   Applications: ${FEELPP_ENABLE_APPLICATIONS}")
MESSAGE(STATUS "     Benchmarks: ${FEELPP_ENABLE_BENCHMARKS}")
MESSAGE(STATUS "       Research: ${FEELPP_ENABLE_RESEARCH}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Feel++ Configuration:")
MESSAGE(STATUS "                CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "              CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "                 CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "         CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
MESSAGE(STATUS "  CMAKE_CXX_FLAGS_RELWITHDEBINFO: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
MESSAGE(STATUS "           CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE(STATUS "              FEELPP_INCLUDE_DIR: ${FEELPP_INCLUDE_DIR}")
MESSAGE(STATUS "                 FEELPP_DATA_DIR: ${FEELPP_DATA_DIR}")
MESSAGE(STATUS "")
MESSAGE(STATUS "             FEELPP_MACHINE_NAME: ${FEELPP_MACHINE_NAME}")
MESSAGE(STATUS "               FEELPP_ENABLE_GIT: ${FEELPP_ENABLE_GIT}")
MESSAGE(STATUS "          FEELPP_ENABLE_MPI_MODE: ${FEELPP_ENABLE_MPI_MODE} Nprocs: ${N} Nprocs(build): ${N2}")
MESSAGE(STATUS "           FEELPP_MESH_MAX_ORDER: ${FEELPP_MESH_MAX_ORDER}")
MESSAGE(STATUS "       FEELPP_INSTANTIATION_MODE: ${FEELPP_INSTANTIATION_MODE}")
MESSAGE(STATUS "  FEELPP_INSTANTIATION_ORDER_MAX: ${FEELPP_INSTANTIATION_ORDER_MAX}")
MESSAGE(STATUS "          FEELPP_ENABLED_OPTIONS: ${FEELPP_ENABLED_OPTIONS}")
MESSAGE(STATUS "         FEELPP_ENABLED_PROJECTS: ${FEELPP_ENABLED_PROJECTS}")
MESSAGE(STATUS "                       SCHEDULER: ${FEELPP_SCHEDULER}")
if (FEELPP_ENABLE_SCHED_OAR )
message(STATUS "            *** MPIEXEC_PREFLAGS:  ${MPIEXEC_PREFLAGS} ONLY VALID FOR OpenMPI ***")
endif()

MESSAGE(STATUS "=================================================================================================")

string(TOLOWER "${CMAKE_GENERATOR}" cmake_generator_tolower)
if(cmake_generator_tolower MATCHES "makefile")
  message(STATUS "Some things you can do now with Feel++:")
  MESSAGE(STATUS "===============================================================================================")
  message(STATUS "Command                  |   Description")
  MESSAGE(STATUS "=========================|=====================================================================")
  if(TARGET feelpp)
      message(STATUS "make feelpp              | Compile the Feel++ library")
  endif()
  if(TARGET install-feelpp-lib)
      message(STATUS "make install-feelpp-lib  | Compile the Feel++ library and install it to ${CMAKE_INSTALL_PREFIX}")
  endif()
  if(TARGET install-feelpp-base)
      message(STATUS "make install-feelpp-base | Compile the Feel++ library and sample applications, then install them to ${CMAKE_INSTALL_PREFIX}")
  endif()
  message(STATUS "make install             | Compile all the components of Feel++ and install them to ${CMAKE_INSTALL_PREFIX}")
  if(TARGET feelpp-models)
      message(STATUS "make feelpp-models       | build feel++ models: fluid, solid, fsi, thermodynamics ")
  endif()
  MESSAGE(STATUS "===============================================================================================")
endif()

message(STATUS "")
