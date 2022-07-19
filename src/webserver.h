/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "seasocks/Response.h"
#include "seasocks/Server.h"
#include "seasocks/StringUtil.h"
#include "seasocks/WebSocket.h"
#include "seasocks/util/PathHandler.h"
#include "seasocks/util/RootPageHandler.h"

struct participant_handler;
struct quizmaster_handler;
class quiz_runner;

struct DataHandler : seasocks::CrackedUriPageHandler {
  virtual std::shared_ptr<seasocks::Response> handle(const seasocks::CrackedUri & /*uri*/,
                                                     const seasocks::Request &request) override;
};

class webserver {
private:
  std::shared_ptr<seasocks::Server> server_ = nullptr;
  std::shared_ptr<quiz_runner> quiz_runner_ = nullptr;
  std::shared_ptr<participant_handler> participant_handler_ = nullptr;
  std::shared_ptr<quizmaster_handler> quizmaster_handler_ = nullptr;

public:
  explicit webserver();

  void run();
  void stop();
};
