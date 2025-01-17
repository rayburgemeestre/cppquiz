/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "logger.h"

std::ofstream logger::fi{"app.log", std::ios_base::out | std::ios_base::app};

bool _stdout = true;