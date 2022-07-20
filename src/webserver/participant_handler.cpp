/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "webserver/participant_handler.h"
#include "fmt/core.h"
#include "nlohmann/json.hpp"
#include "quiz_runner.h"
#include "util/logger.h"
#include "webserver.h"

using json = nlohmann::json;

participant_handler::participant_handler(std::shared_ptr<seasocks::Server> server,
                                         std::shared_ptr<quiz_runner> quiz_runner)
    : server_(server), quiz_runner_(quiz_runner) {
  quiz_runner_->add_participant_callback([&](nlohmann::json msg) {
    if (msg["msg"] == "start_quiz" || msg["msg"] == "set_question" || msg["msg"] == "set_answering_time" ||
        msg["msg"] == "solution") {
      for (auto &con : connections_) {
        server_->execute([=]() {
          con->send(msg.dump());
        });
      }
    }
  });
}

void participant_handler::onConnect(seasocks::WebSocket *con) {
  connections_.insert(con);
}

void participant_handler::onDisconnect(seasocks::WebSocket *con) {
  connections_.erase(con);
  if (con_to_unique_id_.contains(con)) {
    quiz_runner_->set_participant_connected(con_to_unique_id_[con], false);
    con_to_unique_id_.erase(con);
  }
}

void participant_handler::onData(seasocks::WebSocket *con, const char *data) {
  std::string input(data);
  try {
    auto client_msg = nlohmann::json::parse(input);
    if (client_msg["msg"] == "hello") {
      con->send(nlohmann::json{
          {"msg", "hello"},
          {"value", "Hi there!"},
          {"quiz_started", quiz_runner_->quiz_started()},
          {"quiz_id", quiz_runner_->quiz_id()},
          {"current_question", quiz_runner_->current_question_json()},
          {"answering_time", quiz_runner_->answering_time()},
      }
                    .dump());
    } else if (client_msg["msg"] == "set_nickname") {
      quiz_runner_->set_participant_connected(client_msg["unique_id"], true);
      quiz_runner_->add_participant(client_msg["unique_id"], client_msg["nickname"]);
      con_to_unique_id_.insert_or_assign(con, client_msg["unique_id"]);
      con->send(nlohmann::json{
          {"msg", "set_nickname"},
          {"value", fmt::format("Nice to meet you {}!", client_msg["nickname"])},
      }
                    .dump());
      quiz_runner_->send_participants_to_quizmaster();
    } else if (client_msg["msg"] == "set_answers") {
      quiz_runner_->set_answers(client_msg["unique_id"], client_msg["answers"]);
    }
  } catch (const nlohmann::json::exception &e) {
    logger(ERROR) << "Error parsing JSON: " << e.what() << std::endl;
  }
}
