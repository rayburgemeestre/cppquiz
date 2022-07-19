/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "webserver/quizmaster_handler.h"
#include "nlohmann/json.hpp"
#include "quiz_runner.h"
#include "util/logger.h"
#include "webserver.h"

using json = nlohmann::json;

quizmaster_handler::quizmaster_handler(std::shared_ptr<quiz_runner> quiz_runner) : quiz_runner_(quiz_runner) {
  quiz_runner_->on_new_participant([&](std::string unique_id) {
    for (auto &con : confirmed_connections_) {
      con->send(nlohmann::json({{"msg", "new_participant"}, {"value", unique_id}}).dump());
    }
    send_participants();
  });
}

void quizmaster_handler::onConnect(seasocks::WebSocket *con) {
  connections_.insert(con);
}

void quizmaster_handler::onDisconnect(seasocks::WebSocket *con) {
  connections_.erase(con);
  confirmed_connections_.erase(con);
}

void quizmaster_handler::onData(seasocks::WebSocket *con, const char *data) {
  std::string input(data);
  try {
    auto json = nlohmann::json::parse(input);
    if (json["msg"] == "hello") {
      if (quizmaster_uuid_.empty()) {
        quizmaster_uuid_ = json["unique_id"];
      }
      if (quizmaster_uuid_ == json["unique_id"]) {
        confirmed_connections_.insert(con);
        con->send(nlohmann::json{
            {"msg", "hello"},
            {"value", "Hi there Mr. Admin!"},
        }
                      .dump());
        send_participants();
      } else {
        con->send(nlohmann::json{
            {"msg", "hello"},
            {"value", "Sorry, there is already a different quizmaster!"},
        }
                      .dump());
        con->close();
      }
    }
  } catch (const nlohmann::json::exception &e) {
    logger(ERROR) << "Error parsing JSON: " << e.what() << std::endl;
  }
}

void quizmaster_handler::send_participants() {
  for (auto &con : confirmed_connections_) {
    nlohmann::json participants;
    for (const auto &[_, participant] : quiz_runner_->get_participants()) {
      participants.push_back(nlohmann::json{{"unique_id", participant.unique_id()},
                                            {"nickname", participant.nickname()},
                                            {"connected", participant.connected()}});
    }
    con->send(nlohmann::json{{"msg", "participants"}, {"value", participants}}.dump());
  }
}
