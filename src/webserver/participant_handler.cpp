/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "webserver/participant_handler.h"
#include "fmt/core.h"
#include "nlohmann/json.hpp"
#include "util/logger.h"
#include "webserver.h"

using json = nlohmann::json;

participant_handler::participant_handler() {}

void participant_handler::onConnect(seasocks::WebSocket *con) {
  connections_.insert(con);
}

void participant_handler::onDisconnect(seasocks::WebSocket *con) {
  connections_.erase(con);
}

void participant_handler::onData(seasocks::WebSocket *con, const char *data) {
  std::string input(data);
  logger(DEBUG) << "quizmaster REQ " << input << std::endl;
  try {
    auto client_msg = nlohmann::json::parse(input);
    if (client_msg["msg"] == "hello") {
      nlohmann::json response;
      response["msg"] = "hello";
      response["value"] = "Hi there!";
      logger(DEBUG) << "quizmaster REP " << response.dump() << std::endl;
      con->send(response.dump());
    } else if (client_msg["msg"] == "set_nickname") {
      nlohmann::json response;
      response["msg"] = "set_nickname";
      response["value"] = fmt::format("Nice to meet you {}!", client_msg["nickname"]);
      logger(DEBUG) << "quizmaster REP " << response.dump() << std::endl;
      con->send(response.dump());
    }
  } catch (const nlohmann::json::exception &e) {
    logger(ERROR) << "Error parsing JSON: " << e.what() << std::endl;
  }
}
