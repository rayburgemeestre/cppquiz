/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "seasocks/Server.h"

class quiz_runner;

struct participant_handler : seasocks::WebSocket::Handler {
  std::shared_ptr<seasocks::Server> server_;
  std::set<seasocks::WebSocket *> connections_;
  std::map<seasocks::WebSocket *, std::string> con_to_unique_id_;
  std::shared_ptr<quiz_runner> quiz_runner_ = nullptr;

  participant_handler(std::shared_ptr<seasocks::Server> server, std::shared_ptr<quiz_runner> quiz_runner);

  void onConnect(seasocks::WebSocket *con) override;
  void onDisconnect(seasocks::WebSocket *con) override;
  void onData(seasocks::WebSocket *con, const char *data) override;
};
