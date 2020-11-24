// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @file   BHnHolder.h
/// @author Thomas Klemenz, thomas.klemenz@tum.de
///

#ifndef AliceO2_TPC_BHNHOLDER_H
#define AliceO2_TPC_BHNHOLDER_H

#include "TPCBase/BHn.h"

namespace o2
{
namespace tpc
{

/// @brief  BHnHolder class
///
/// Container of n-dimensional boost histogram class objects
/// origin: TPC
/// @author Thomas Klemenz, thomas.klemenz@tum.de
class BHnHolder
{
 public:
  /// default constructor
  BHnHolder() = default;
  ~BHnHolder() = default;

 private:

  ClassDefNV(BHnHolder, 1)
};
} // namespace tpc
} // namespace o2

#endif