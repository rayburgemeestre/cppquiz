/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "webserver/quizmaster_handler.h"
#include "nlohmann/json.hpp"
#include "util/logger.h"
#include "webserver.h"

using json = nlohmann::json;

quizmaster_handler::quizmaster_handler() {}

void quizmaster_handler::onConnect(seasocks::WebSocket *con) {
  connections_.insert(con);
}

void quizmaster_handler::onDisconnect(seasocks::WebSocket *con) {
  connections_.erase(con);
}

void quizmaster_handler::onData(seasocks::WebSocket *con, const char *data) {
  std::string input(data);
  try {
    auto json = nlohmann::json::parse(input);
    if (json["msg"] == "hello") {
      nlohmann::json response;
      response["msg"] = "Hi there Mr. Admin!";
      con->send(response.dump());
    }
  } catch (const nlohmann::json::exception &e) {
    logger(ERROR) << "Error parsing JSON: " << e.what() << std::endl;
  }
}
