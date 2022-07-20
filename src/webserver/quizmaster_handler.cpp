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
  quiz_runner_->add_quizmaster_callback([&](nlohmann::json msg) {
    if (msg["msg"] == "add_participant") {
      for (auto &con : confirmed_connections_) {
        con->send(msg.dump());
      }
      quiz_runner_->send_participants_to_quizmaster();
    } else if (msg["msg"] == "set_question") {
      for (auto &con : confirmed_connections_) {
        con->send(msg.dump());
      }
    } else if (msg["msg"] == "answers_updated") {
      for (auto &con : confirmed_connections_) {
        con->send(msg.dump());
      }
    } else if (msg["msg"] == "participants") {
      for (auto &con : confirmed_connections_) {
        con->send(msg.dump());
      }
    }
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
            {"quiz_started", quiz_runner_->quiz_started()},
            {"quiz_id", quiz_runner_->quiz_id()},
            {"current_question", quiz_runner_->current_question_json()},
            {"current_question_id", quiz_runner_->current_question()},
            {"num_questions", quiz_runner_->num_questions()},
        }
                      .dump());
        quiz_runner_->send_participants_to_quizmaster();
        quiz_runner_->send_answers_to_quizmaster();
      } else {
        con->send(nlohmann::json{
            {"msg", "hello"},
            {"value", "Sorry, there is already a different quizmaster!"},
        }
                      .dump());
        con->close();
      }
    } else if (quizmaster_uuid_ == json["unique_id"]) {
      if (json["msg"] == "start_quiz") {
        quiz_runner_->start_quiz(json["quiz_id"]);
        con->send(nlohmann::json{
            {"msg", "quiz_started"},
            {"value", quiz_runner_->quiz_started()},
        }
                      .dump());
      } else if (json["msg"] == "stop_quiz") {
        logger(INFO) << "Stop quiz" << std::endl;
        quiz_runner_->stop_quiz();
        con->send(nlohmann::json{
            {"msg", "quiz_started"},
            {"value", quiz_runner_->quiz_started()},
        }
                      .dump());
      } else if (json["msg"] == "next_question") {
        quiz_runner_->next_question();
      }
    }
  } catch (const nlohmann::json::exception &e) {
    logger(ERROR) << "Error parsing JSON: " << e.what() << std::endl;
  }
}