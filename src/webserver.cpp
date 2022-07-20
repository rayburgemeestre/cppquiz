/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "webserver.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include "nlohmann/json.hpp"
#include "quiz_runner.h"
#include "seasocks/Logger.h"
#include "util/logger.h"
#include "webserver/participant_handler.h"
#include "webserver/quizmaster_handler.h"

using json = nlohmann::json;

std::shared_ptr<seasocks::Response> DataHandler::handle(const seasocks::CrackedUri & /*uri*/,
                                                        const seasocks::Request &request) {
  return seasocks::Response::jsonResponse("{}");
}

class MyLogger : public seasocks::Logger {
public:
  explicit MyLogger(Level minLevelToLog_ = Level::Debug) : minLevelToLog(minLevelToLog_) {}
  ~MyLogger() = default;
  virtual void log(Level level, const char *message) override {
    if (level >= minLevelToLog) {
      switch (level) {
        case Level::Access:
          logger(DEBUG) << "[access]" << message << std::endl;
          return;
        case Level::Debug:
          logger(DEBUG) << message << std::endl;
          return;
        case Level::Info:
          logger(INFO) << message << std::endl;
          return;
        case Level::Warning:
        case Level::Error:
          logger(WARNING) << message << std::endl;
          return;
        case Level::Severe:
          logger(FATAL) << message << std::endl;
          return;
      }
    }
  }
  Level minLevelToLog;
};

std::vector<question> read_questions_from_file(std::string filename) {
  std::vector<question> questions;

  std::ifstream fi(filename);
  std::string line;
  std::string current_question;
  std::vector<answer> current_answers;
  while (std::getline(fi, line)) {
    if (line.empty()) continue;
    if (line[0] == 'Q') {
      if (!current_question.empty()) {
        questions.emplace_back(current_question, current_answers);
        current_answers.clear();
      }
      current_question = line.substr(3, line.size() - 3);
      current_answers.clear();
    } else if (line[0] == '*') {
      auto current_answer = line.substr(4, line.size() - 4);
      current_answers.emplace_back(current_answer, true);
    } else if (line[0] == 'A') {
      auto current_answer = line.substr(3, line.size() - 3);
      current_answers.emplace_back(current_answer, false);
    }
  }
  if (!current_question.empty()) {
    questions.emplace_back(current_question, current_answers);
    current_answers.clear();
  }
  fi.close();

  return questions;
}

webserver::webserver()
    : server_(std::make_shared<seasocks::Server>(std::make_shared<MyLogger>(MyLogger::Level::Info))),
      quiz_runner_(std::make_shared<quiz_runner>(quiz("The Big C++ Quiz", read_questions_from_file("quiz.txt")))),
      participant_handler_(std::make_shared<participant_handler>(server_, quiz_runner_)),
      quizmaster_handler_(std::make_shared<quizmaster_handler>(server_, quiz_runner_)) {
  auto root = std::make_shared<seasocks::RootPageHandler>();
  root->add(std::make_shared<seasocks::PathHandler>("data", std::make_shared<DataHandler>()));
  server_->addPageHandler(root);
  server_->setClientBufferSize(6653847300);
  server_->addWebSocketHandler("/participant", participant_handler_);
  server_->addWebSocketHandler("/quizmaster", quizmaster_handler_);
}

void webserver::run() {
  std::string str = "web";
  server_->serve(str.c_str(), 18080);
}

void webserver::stop() {
  server_->terminate();
}